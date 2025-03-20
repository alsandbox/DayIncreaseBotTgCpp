#ifndef BOTMANAGER_H
#define BOTMANAGER_H

#include <tgbot/tgbot.h>
#include <memory>
#include "WeatherApiManager.h"
#include "MessageHandler.h"
#include "LocationService.h"
#include "UpdateScheduler.h"

class BotManager
{
public:
    BotManager(const std::shared_ptr<TgBot::Bot>& bot, const std::shared_ptr<WeatherApiManager>& weatherApiManager,
               const std::shared_ptr<WeatherDataParser>& weatherDataParser);
    void startBot();
    static void handlePollingError(const std::exception& exception);

private:
    std::shared_ptr<TgBot::Bot> bot_;
    std::shared_ptr<WeatherApiManager> weatherApiManager_;
    std::shared_ptr<WeatherDataParser> weatherDataParser_;
    std::shared_ptr<LocationService> locationService_;
    std::shared_ptr<UpdateScheduler> updateScheduler_;
    std::shared_ptr<MessageHandler> messageHandler_;
    bool isRunning = false;
};

#endif
