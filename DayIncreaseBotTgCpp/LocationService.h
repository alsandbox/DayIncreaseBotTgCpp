#ifndef LOCATIONSERVICE_H
#define LOCATIONSERVICE_H

#include <tgbot/tgbot.h>
#include <memory>
#include "WeatherApiManager.h"

class LocationService
{
public:
    LocationService(const std::shared_ptr<TgBot::Bot>& bot,
                    const std::shared_ptr<WeatherApiManager>& weatherApiManager);
    void requestLocation(int64_t chatId) const;
    void handleLocationReceived(int64_t chatId, const TgBot::Message::Ptr& message);
    std::function<void()> onLocationReceived;
    [[nodiscard]] bool getIfLocationIsAvailable() const { return isLocationReceived; }
private:
    std::shared_ptr<TgBot::Bot> bot_;
    std::shared_ptr<WeatherApiManager> weatherApiManager_;
    bool isLocationReceived = false;
};
#endif
