#pragma once
#include <chrono>
#include <memory>
#include <string>

#include "SolsticeData.h"
#include "WeatherApiClient.h"

class WeatherApiManager
{
public:
    WeatherApiManager(std::shared_ptr<WeatherApiClient> const& weatherApiClient);

    double GetLatitude() const { return latitude; }
    double GetLongitude() const { return longitude; }

    double SetLatitude(double latitude_)
    {
        latitude = latitude_;
        return latitude;
    }

    double SetLongitude(double longitude_)
    {
        longitude = longitude_;
        return longitude;
    }

    std::string getTimeAsync(std::chrono::system_clock::time_point date);

private:
    std::shared_ptr<WeatherApiClient> weatherApiClient_;
    double latitude;
    double longitude;
};
