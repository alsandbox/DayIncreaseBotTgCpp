#ifndef WEATHER_API_CLIENT_H
#define WEATHER_API_CLIENT_H
#include <string>

class WeatherApiClient
{
public:
    explicit WeatherApiClient(const std::string& apiUrl, const std::string& tzApiUrl, const std::string& apiKey);
    [[nodiscard]] std::string getTzId(double latitude, double longitude) const;
    [[nodiscard]] std::string getWeatherData(double latitude, double longitude, const std::string& date, const std::string& tzId) const;

private:
    std::string m_apiUrl;
    std::string m_tzApiUrl;
    std::string m_apiKey;
};
#endif
