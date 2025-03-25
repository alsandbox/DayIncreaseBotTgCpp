#include "BotManager.h"
#include <tgbot/tgbot.h>
#include <iostream>
#include <future>

BotManager::BotManager(const std::shared_ptr<TgBot::Bot>& bot,
                       const std::shared_ptr<WeatherApiManager>& weatherApiManager,
                       const std::shared_ptr<WeatherDataParser>& weatherDataParser,
                       std::atomic<bool>& isRunning)
    : bot_(bot),
      weatherApiManager_(weatherApiManager),
      weatherDataParser_(weatherDataParser),
      locationService_(std::make_shared<LocationService>(bot, weatherApiManager_)),
      updateScheduler_(std::make_shared<UpdateScheduler>(bot, weatherApiManager_, weatherDataParser_)),
      messageHandler_(std::make_shared<MessageHandler>(weatherApiManager_, locationService_, bot, updateScheduler_)),
      isRunning_(isRunning)
{
}

void BotManager::startBot() const {
    std::cout << "Bot is starting..." << std::endl;
    StateInfo stateInfo_;

    bot_->getEvents().onAnyMessage([this, stateInfo_](const TgBot::Message::Ptr& message) {
        const int64_t chatId = message->chat->id;
        const auto stateDict = messageHandler_->getStateInfo(chatId);

        if (stateDict == nullptr) {
            messageHandler_->setStateInfo(chatId, stateInfo_);
        }

        messageHandler_->handleUpdate(chatId, message);
    });

    (void)std::async(std::launch::async, [this]() {
        try {
            TgBot::TgLongPoll longPoll(*bot_);
            while (isRunning_) {
                longPoll.start();
            }
            std::cout << "Bot stopped gracefully.\n";
        } catch (const std::exception& e) {
            handlePollingError(e);
        }
    });
}

void BotManager::handlePollingError(const std::exception& exception)
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