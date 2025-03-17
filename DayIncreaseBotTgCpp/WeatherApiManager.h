#ifndef WEATHER_API_MANAGER_H
#define WEATHER_API_MANAGER_H
#include <chrono>
#include <memory>
#include <string>

#include "WeatherApiClient.h"

class WeatherApiManager
{
public:
    explicit WeatherApiManager(const std::shared_ptr<WeatherApiClient>& weatherApiClient);

    [[nodiscard]] double getLatitude() const { return latitude; }
    [[nodiscard]] double getLongitude() const { return longitude; }

    double setLatitude(const double latitude_)
    {
        latitude = latitude_;
        return latitude;
    }

    double setLongitude(const double longitude_)
    {
        longitude = longitude_;
        return longitude;
    }

    [[nodiscard]] std::string getTime(std::chrono::system_clock::time_point date) const;

private:
    std::shared_ptr<WeatherApiClient> weatherApiClient_;
    double latitude = 0;
    double longitude = 0;
    std::string apiUrl;
};
#endif
