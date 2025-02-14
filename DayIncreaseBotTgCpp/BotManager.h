#ifndef BOTMANAGER_H
#define BOTMANAGER_H

#include <tgbot/tgbot.h>
#include <memory>
#include <atomic>
#include "WeatherApiManager.h"
#include "MessageHandler.h"
#include "LocationService.h"
#include "UpdateScheduler.h"

class BotManager {
public:
    BotManager(std::shared_ptr<TgBot::Bot> bot, std::shared_ptr<WeatherApiManager> weather_api_manager);

    void startBot();
    void stopBot();

private:
    const std::shared_ptr<TgBot::Bot> bot_;
    std::shared_ptr<WeatherApiManager> weather_api_manager_;
    std::shared_ptr<LocationService> location_service_;
    std::shared_ptr<UpdateScheduler> update_scheduler_;
    std::shared_ptr<MessageHandler> message_handler_;
    std::atomic<bool> is_running_;
};

#endif
