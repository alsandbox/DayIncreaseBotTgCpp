#include "BotManager.h"
#include <tgbot/tgbot.h>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>

BotManager::BotManager(std::shared_ptr<TgBot::Bot> bot, const std::shared_ptr<WeatherApiManager>& weatherApiManager)
    : bot_(bot),
      weatherApiManager_(weatherApiManager),
      locationService_(std::make_shared<LocationService>(bot, weatherApiManager_)),
      updateScheduler_(std::make_shared<UpdateScheduler>(bot, weatherApiManager_)),
      messageHandler_(std::make_shared<MessageHandler>(weatherApiManager_, locationService_, bot, updateScheduler_)),
      isRunning(false) {}

void BotManager::startBot() {
    std::cout << "Bot is starting..." << '\n';
    isRunning = true;

    std::thread botThread([this]() {
        try {
            TgBot::TgLongPoll longPoll(*bot_);
            while (is_running_) {
                longPoll.start();
            }
        } catch (const std::exception& e) {
            std::cerr << "Bot encountered an error: " << e.what() << '\n';
        }
    });

    botThread.join();
    std::cout << "Bot is stopping..." << '\n';
}

void BotManager::stopBot() {
    is_running_ = false;
}
