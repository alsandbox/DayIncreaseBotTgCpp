#include "MessageHandler.h"

#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>
#include <tgbot/tgbot.h>

MessageHandler::MessageHandler(const std::shared_ptr<WeatherApiManager> &weatherApiManager,
                               const std::shared_ptr<LocationService> &locationService,
                               const std::shared_ptr<TgBot::Bot> &bot,
                               const std::shared_ptr<UpdateScheduler> &updateScheduler)
    : m_weatherApiManager(weatherApiManager),
      m_locationService(locationService),
      m_updateScheduler(updateScheduler),
      m_bot(bot) {
}

void MessageHandler::handleGetTodayInfo(int64_t chatId) const {
    const auto now = std::chrono::system_clock::now();
    const std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);

    std::tm localTime = {};

    localtime_r(&time_t_now, &localTime);

    const std::chrono::system_clock::time_point today = std::chrono::system_clock::from_time_t(std::mktime(&localTime));

    if (std::string weather_data_json = m_weatherApiManager->getTime(today); !weather_data_json.empty()) {
        try {
            const nlohmann::json weatherJson = nlohmann::json::parse(weather_data_json);

            auto [sunriseTime, sunsetTime, dayLength] = WeatherData::fromJson(weatherJson);
            const std::string message = "Sunrise time: " + sunriseTime +
                                        "\nSunset time: " + sunsetTime +
                                        "\nThe day length: " + dayLength;
            (void) m_bot->getApi().sendMessage(chatId, message);
        } catch (const std::exception &e) {
            std::cerr << "Error parsing weather data: " << e.what() << '\n';
            (void) m_bot->getApi().sendMessage(chatId, "Unable to retrieve weather data.");
        }
    } else {
        std::cout << "Error fetching weather data" << '\n';
        (void) m_bot->getApi().sendMessage(chatId, "Unable to fetch weather data.");
    }
}

void MessageHandler::handleDaylightInfo(int64_t chatId) const {
    if (m_weatherApiManager->getLatitude() <= 0 && m_weatherApiManager->getLongitude() <= 0) {
        if (const std::shared_ptr<TgBot::Chat> chat = m_bot->getApi().getChat(chatId);
            chat->type == TgBot::Chat::Type::Private) {
            m_locationService->requestLocation(chatId);
        } else {
            (void) m_bot->getApi().sendMessage(chatId, "Please send the location.",
                                               nullptr);
        }
    }

    if (m_locationService->getIfLocationIsAvailable()) {
        m_updateScheduler->sendDailyMessage(chatId);

        if (m_updateScheduler->getIsDaylightIncreasing()) {
            handleGetTodayInfo(chatId);
        } else {
            (void) m_bot->getApi().sendMessage(chatId, "Daylight hours are shortening, wait for the winter solstice.",
                                               nullptr);
        }
    }
}

void MessageHandler::handleUpdate(int64_t chatId, const TgBot::Message::Ptr &message) {
    try {
        if (message) {
            if (!message->text.empty()) {
                std::string command = message->text.substr(0, message->text.find(' '));

                if (const size_t atIndex = command.find('@'); atIndex != std::string::npos) {
                    command = command.substr(0, atIndex);
                }
                selectCommand(command, chatId, message);
            }
            if (message->chat->type != TgBot::Chat::Type::Private && m_isSentOnce == false && m_isLocationRequired) {
                (void) m_bot->getApi().sendMessage(chatId, "Please, send me your location replying on this message.",
                                                   nullptr);
                m_isSentOnce = true;
            } else if (
                (message->chat->type != TgBot::Chat::Type::Private) ||
                ((message->location && message->chat->type == TgBot::Chat::Type::Private) && m_isLocationRequired)
            ) {
                handleLocation(message, chatId);
            }
        }
    } catch (const TgBot::TgException &e) {
        if (const std::string errorMsg = e.what(); errorMsg.find("query is too old") != std::string::npos) {
            std::cout << "API request error: " << errorMsg << '\n';
        }
    } catch (const std::exception &e) {
        std::cout << "Unhandled exception in HandleUpdateAsync: " << e.what() << '\n';
    }
}

void MessageHandler::scheduleDaylightUpdateWithLambda(int64_t chatId) const {
    auto daylightUpdateLambda = [this, chatId] {
        handleDaylightInfo(chatId);
    };

    m_updateScheduler->scheduleUvUpdates(daylightUpdateLambda);
}

void MessageHandler::selectCommand(const std::string &command, int64_t chatId, const TgBot::Message::Ptr &message) {
    auto it = m_stateInfoDictionary.find(chatId);
    it->second.lastCommand = command;

    if (command == "/start") {
        (void) m_bot->getApi().sendMessage(chatId, "Bot started! Use available commands to interact.", nullptr);
    } else if (command == "/gettodaysinfo") {
        m_isLocationRequired = true;
        if (it->second.hasLocation) {
            handleDaylightInfo(chatId);
            it->second.hasLocation = m_locationService->getIfLocationIsAvailable();
        } else {
            m_locationService->onLocationReceived = [this, chatId, it] {
                if (it->second.lastCommand == "/gettodaysinfo") {
                    it->second.lastCommand.clear();
                    handleDaylightInfo(chatId);
                }
            };

            askLocationDependingChatType(message, chatId);
        }
    } else if (command == "/changelocation") {
        m_isLocationRequired = true;
        m_locationService->onLocationReceived = [it] {
            if (it->second.lastCommand == "/changelocation") {
                it->second.lastCommand.clear();
            }
        };
        askLocationDependingChatType(message, chatId);
    } else if (command == "/getdaystillsolstice") {
        m_updateScheduler->handleDaysTillSolstice(chatId);
    } else if (command == "/setinterval") {
        m_isLocationRequired = true;

        if (it->second.hasLocation) {
            scheduleDaylightUpdateWithLambda(chatId);
            (void) m_bot->getApi().sendMessage(
                chatId,
                "The next message will be sent after 24 hours. You will receive messages every day until the summer solstice.",
                nullptr);
            it->second.hasLocation = m_locationService->getIfLocationIsAvailable();
        } else {
            m_locationService->onLocationReceived = [this, chatId, it] {
                if (it->second.lastCommand == "/setinterval") {
                    it->second.lastCommand.clear();
                    scheduleDaylightUpdateWithLambda(chatId);
                    (void) m_bot->getApi().sendMessage(
                        chatId,
                        "The next message will be sent after 24 hours. You will receive messages every day until the summer solstice.",
                        nullptr);
                }
            };

            askLocationDependingChatType(message, chatId);
        }
    } else if (command == "/cancelinterval") {
        m_updateScheduler->cancelUvUpdates();
        (void) m_bot->getApi().sendMessage(chatId, "The interval is cancelled.", nullptr);
    }
}

void MessageHandler::handleLocation(const TgBot::Message::Ptr &message, int64_t chatId) {
    const auto it = m_stateInfoDictionary.find(chatId);

    if (message && message->location) {
        auto future = std::async(std::launch::async,
                                 &LocationService::handleLocationReceived,
                                 m_locationService.get(),
                                 chatId,
                                 message);

        future.get();
        it->second.hasLocation = m_locationService->getIfLocationIsAvailable();
        if (m_locationService->onLocationReceived) {
            m_locationService->onLocationReceived();
            m_locationService->onLocationReceived = nullptr;
        }
    }
}

void MessageHandler::askLocationDependingChatType(const TgBot::Message::Ptr &message, int64_t chatId) {
    if (message->chat->type != TgBot::Chat::Type::Private && m_isSentOnce == false) {
        (void) m_bot->getApi().sendMessage(chatId, "Please, send me your location replying on this message.", nullptr);
        m_isSentOnce = true;
    } else if (message->chat->type == TgBot::Chat::Type::Private) {
        m_locationService->requestLocation(chatId);
    }
}
