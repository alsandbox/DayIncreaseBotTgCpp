#ifndef LOCATIONSERVICE_H
#define LOCATIONSERVICE_H

#include <tgbot/tgbot.h>
#include <memory>
#include <atomic>
#include "WeatherApiManager.h"


class LocationService
{
public:
    void requestLocation(long chatId) const;
    void handleLocationReceived(long chatId, std::atomic<bool>& cancellation_token, const TgBot::Message::Ptr& message);
    LocationService(const std::shared_ptr<TgBot::Bot>& bot,
                    const std::shared_ptr<WeatherApiManager>& weatherApiManager);
    void requestLocation(int64_t chatId) const;
    void handleLocationReceived(int64_t chatId, const TgBot::Message::Ptr& message);
    std::function<void()> onLocationReceived;
    
private:
    std::shared_ptr<TgBot::Bot> bot_;
    std::shared_ptr<WeatherApiManager> weatherApiManager_;
    bool isLocationReceived = false;
};
#endif
