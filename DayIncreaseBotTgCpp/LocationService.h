#ifndef LOCATIONSERVICE_H
#define LOCATIONSERVICE_H

#include <tgbot/tgbot.h>
#include <memory>
#include <atomic>
#include "WeatherApiManager.h"


class LocationService
{
public:
    LocationService(const std::shared_ptr<TgBot::Bot> bot, std::shared_ptr<WeatherApiManager> const & weather_api_manager);
    void RequestLocation(long chat_id) const;
    void HandleLocationReceived(long chat_id, std::atomic<bool>& cancellation_token, const TgBot::Message::Ptr& message);
    std::function<void()> on_location_received;
    
private:
    std::shared_ptr<TgBot::Bot> bot_;
    std::shared_ptr<WeatherApiManager> weather_api_manager_;
    bool is_location_received = false;
};
#endif  