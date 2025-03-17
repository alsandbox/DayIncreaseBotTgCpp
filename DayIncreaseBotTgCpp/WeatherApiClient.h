#ifndef WEATHER_API_CLIENT_H
#define WEATHER_API_CLIENT_H
#include <string>

class WeatherApiClient
{
public:
    explicit WeatherApiClient(const std::string& apiUrl);
    [[nodiscard]] std::string getWeatherData(double latitude, double longitude, const std::string& date) const;

private:
    std::string apiUrl_;
};
#endif
