#ifndef WEATHER_API_CLIENT_h
#define WEATHER_API_CLIENT_h
#include <chrono>

class WeatherApiClient
{
public:
    WeatherApiClient(const std::string& apiUrl);
    std::string getWeatherData(double latitude, double longitude, const std::string& date);
private:
    std::string apiUrl;
};
#endif