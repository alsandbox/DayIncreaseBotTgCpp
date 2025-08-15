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
    [[nodiscard]] bool getIfLocationIsAvailable() const { return m_isLocationReceived; }

private:
    std::shared_ptr<TgBot::Bot> m_bot;
    std::shared_ptr<WeatherApiManager> m_weatherApiManager;
    bool m_isLocationReceived = false;
};
#endif
