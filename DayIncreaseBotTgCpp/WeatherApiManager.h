#ifndef WEATHER_API_MANAGER_H
#define WEATHER_API_MANAGER_H
#include <chrono>
#include <memory>
#include <string>

#include "WeatherApiClient.h"

class WeatherApiManager {
public:
    explicit WeatherApiManager(const std::shared_ptr<WeatherApiClient> &weatherApiClient);

    [[nodiscard]] double getLatitude() const { return m_latitude; }
    [[nodiscard]] double getLongitude() const { return m_longitude; }

    [[nodiscard]] std::string getParsedTzId(double latitude, double longitude) const;

    [[nodiscard]] std::string getTime(std::chrono::system_clock::time_point date) const;

    double setLatitude(const double latitude_) {
        m_latitude = latitude_;
        return m_latitude;
    }

    double setLongitude(const double longitude_) {
        m_longitude = longitude_;
        return m_longitude;
    }

private:
    std::shared_ptr<WeatherApiClient> m_weatherApiClient;
    double m_latitude = 0;
    double m_longitude = 0;
};
#endif
