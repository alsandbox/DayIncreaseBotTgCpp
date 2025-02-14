#include "BotManager.h"
#include <tgbot/tgbot.h>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>

BotManager::BotManager(std::shared_ptr<TgBot::Bot> bot, std::shared_ptr<WeatherApiManager> weatherApiManager)
    : bot_(bot),
      weather_api_manager_(weatherApiManager),
      location_service_(std::make_shared<LocationService>(bot, weather_api_manager_)),
      update_scheduler_(std::make_shared<UpdateScheduler>(bot, weather_api_manager_)),
      message_handler_(std::make_shared<MessageHandler>(weather_api_manager_, location_service_, bot, update_scheduler_)),
      is_running_(false) {}

void BotManager::startBot() {
    std::cout << "Bot is starting..." << '\n';
    is_running_ = true;

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
