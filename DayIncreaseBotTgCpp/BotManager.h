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
    BotManager(std::shared_ptr<TgBot::Bot> bot, const std::shared_ptr<WeatherApiManager>& weatherApiManager);

    void startBot();
    void stopBot();

private:
    const std::shared_ptr<TgBot::Bot> bot_;
    std::shared_ptr<WeatherApiManager> weatherApiManager_;
    std::shared_ptr<LocationService> locationService_;
    std::shared_ptr<UpdateScheduler> updateScheduler_;
    std::shared_ptr<MessageHandler> messageHandler_;
    std::atomic<bool> isRunning;
};

#endif
