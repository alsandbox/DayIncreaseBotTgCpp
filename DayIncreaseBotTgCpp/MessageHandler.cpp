#include "MessageHandler.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <ctime>
#include <tgbot/tgbot.h>

MessageHandler::MessageHandler(const std::shared_ptr<WeatherApiManager>& weatherApiManager,
                               const std::shared_ptr<LocationService>& locationService,
                               const std::shared_ptr<TgBot::Bot>& bot,
                               const std::shared_ptr<UpdateScheduler>& updateScheduler)
    : weatherApiManager_(weatherApiManager),
      locationService_(locationService),
      updateScheduler_(updateScheduler),
      bot_(bot)
{
}

void MessageHandler::handleGetTodayInfo(int64_t chatId) const
{
    const auto now = std::chrono::system_clock::now();
    std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);

    std::tm localTime = {};

#if defined(_WIN32) || defined(_WIN64)
    if (const errno_t err = localtime_s(&localTime, &time_t_now); err != 0)
    {
        std::cerr << "Error converting time: " << err << '\n';
    }
#else
    localtime_r(&time_t_now, &localTime);
#endif

    const std::chrono::system_clock::time_point today = std::chrono::system_clock::from_time_t(std::mktime(&localTime));

    if (std::string weather_data_json = weatherApiManager_->getTime(today); !weather_data_json.empty())
    {
        try
        {
            const nlohmann::json weatherJson = nlohmann::json::parse(weather_data_json);
            
            auto [sunriseTime, sunsetTime, dayLength] = WeatherData::fromJson(weatherJson);
            const std::string message = "Sunrise time: " + sunriseTime +
                "\nSunset time: " + sunsetTime +
                "\nThe day length: " + dayLength;
            (void)bot_->getApi().sendMessage(chatId, message);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing weather data: " << e.what() << '\n';
            (void)bot_->getApi().sendMessage(chatId, "Unable to retrieve weather data.");
        }
    }
    else
    {
        std::cout << "Error fetching weather data" << '\n';
        (void)bot_->getApi().sendMessage(chatId, "Unable to fetch weather data.");
    }
}

void MessageHandler::handleDaylightInfo(int64_t chatId) const {
    if (weatherApiManager_->getLatitude() <= 0 && weatherApiManager_->getLongitude() <= 0)
    {
        if (const std::shared_ptr<TgBot::Chat> chat = bot_->getApi().getChat(chatId); chat->type == TgBot::Chat::Type::Private)
        {
            locationService_->requestLocation(chatId);
        }
        else
        {
            (void)bot_->getApi().sendMessage(chatId, "Please send the location.",
                                 nullptr);
        }
    }

    if (locationService_->getIfLocationIsAvailable()) {
        updateScheduler_->sendDailyMessage(chatId);

        if (updateScheduler_->getIsDaylightIncreasing())
        {
            handleGetTodayInfo(chatId);
        }
        else
        {
            (void)bot_->getApi().sendMessage(chatId, "Daylight hours are shortening, wait for the winter solstice.",
                                             nullptr);
        }
    }
}

void MessageHandler::handleUpdate(int64_t chatId, const TgBot::Message::Ptr &message)
{
    try
    {
        if (message)
        {
            if (!message->text.empty())
            {
                std::string command = message->text.substr(0, message->text.find(' '));

                if (const size_t atIndex = command.find('@'); atIndex != std::string::npos)
                {
                    command = command.substr(0, atIndex);
                }
                selectCommand(command, chatId, message);
            }
            if (message->chat->type != TgBot::Chat::Type::Private && isSentOnce == false) {
                (void)bot_->getApi().sendMessage(chatId, "Please, send me your location replying on this message.", nullptr);
                isSentOnce = true;
            }
            else if (message->chat->type != TgBot::Chat::Type::Private || (message->location && message->chat->type == TgBot::Chat::Type::Private)) {
                handleLocation(message, chatId);
            }

        }
    }
    catch (const TgBot::TgException& e)
    {
        if (const std::string errorMsg = e.what(); errorMsg.find("query is too old") != std::string::npos)
        {
            std::cout << "API request error: " << errorMsg << '\n';
        }
    }
    catch (const std::exception& e)
    {
        std::cout << "Unhandled exception in HandleUpdateAsync: " << e.what() << '\n';
    }
}

void MessageHandler::scheduleDaylightUpdateWithLambda(int64_t chatId) const {
    auto daylightUpdateLambda = [this, chatId]
    {
        handleDaylightInfo(chatId);
    };

    updateScheduler_->scheduleUvUpdates(daylightUpdateLambda);
}

void MessageHandler::selectCommand(const std::string& command, int64_t chatId, const TgBot::Message::Ptr& message)
{
    auto it = stateInfoDictionary.find(chatId);
    it->second.lastCommand = command;

    if (command == "/start")
    {
        (void)bot_->getApi().sendMessage(chatId, "Bot started! Use available commands to interact.", nullptr);
    }
    else if (command == "/gettodaysinfo")
    {
        if (it->second.hasLocation)
        {
            handleDaylightInfo(chatId);
            it->second.hasLocation = locationService_->getIfLocationIsAvailable();
        }
        else
        {
            locationService_->onLocationReceived = [this, chatId, it]
            {
                if (it->second.lastCommand == "/gettodaysinfo")
                    {
                    it->second.lastCommand.clear();
                        handleDaylightInfo(chatId);
                    }
            };

            if (message->chat->type != TgBot::Chat::Type::Private && isSentOnce == false) {
                (void)bot_->getApi().sendMessage(chatId, "Please, send me your location replying on this message.", nullptr);
                isSentOnce = true;
            }
            else if (message->chat->type == TgBot::Chat::Type::Private) {

            }
        }
    }
    else if (command == "/changelocation")
    {
        locationService_->onLocationReceived = [this, it]
        {
                if (it->second.lastCommand == "/changelocation")
                {
                     it->second.lastCommand.clear();
                }
        };
        if (message->chat->type != TgBot::Chat::Type::Private && isSentOnce == false) {
            (void)bot_->getApi().sendMessage(chatId, "Please, send me your location replying on this message.", nullptr);
            isSentOnce = true;
        }
        else if (message->chat->type == TgBot::Chat::Type::Private) {
            locationService_->requestLocation(chatId);
        }
    }
    else if (command == "/getdaystillsolstice")
    {
        updateScheduler_->handleDaysTillSolstice(chatId);
    }
    else if (command == "/setinterval")
    {
        if (it->second.hasLocation)
        {
            scheduleDaylightUpdateWithLambda(chatId);
            (void)bot_->getApi().sendMessage(
                chatId,
                "The next message will be sent after 24 hours. You will receive messages every day until the summer solstice.", nullptr);
                it->second.hasLocation = locationService_->getIfLocationIsAvailable();
        }
        else
        {
            locationService_->onLocationReceived = [this, chatId, it]
            {
                    if (it->second.lastCommand == "/setintervals")
                    {
                        it->second.lastCommand.clear();
                        scheduleDaylightUpdateWithLambda(chatId);
                        (void)bot_->getApi().sendMessage(
                            chatId,
                            "The next message will be sent after 24 hours. You will receive messages every day until the summer solstice.", nullptr);
                    }
            };

            if (message->chat->type != TgBot::Chat::Type::Private && isSentOnce == false) {
                (void)bot_->getApi().sendMessage(chatId, "Please, send me your location replying on this message.", nullptr);
                isSentOnce = true;
            }
            else if (message->chat->type == TgBot::Chat::Type::Private) {
                locationService_->requestLocation(chatId);
            }
        }
    }
    else if (command == "/cancelinterval")
    {
        updateScheduler_->cancelUvUpdates();
        (void)bot_->getApi().sendMessage(chatId, "Intervals cancelled.", nullptr);
    }
}

void MessageHandler::handleLocation(const TgBot::Message::Ptr& message, int64_t chatId)
{
    auto it = stateInfoDictionary.find(chatId);

    if (message && message->location)
    {
        auto future = std::async(std::launch::async,
                                 &LocationService::handleLocationReceived,
                                 locationService_.get(),
                                 chatId,
                                 message);

        future.get();
        it->second.hasLocation = locationService_->getIfLocationIsAvailable();
        if (locationService_->onLocationReceived)
        {
            locationService_->onLocationReceived();
            locationService_->onLocationReceived = nullptr;
        }
    }
}

void MessageHandler::handlePollingError(const std::exception& exception)
{
    if (const auto* tgException = &exception)
    {
        std::cerr << "Telegram API Error:\n" << tgException->what() << '\n';
    }
    else
    {
        std::cerr << "Error: " << exception.what() << '\n';
    }
}
