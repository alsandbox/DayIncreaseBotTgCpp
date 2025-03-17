#include "WeatherApiClient.h"

#include <iostream>
#include <string>
#include <cpr/cpr.h>

WeatherApiClient::WeatherApiClient(const std::string& apiUrl)
{
    if (apiUrl.empty()) {
        throw std::invalid_argument("API URL not configured");
    }
    apiUrl_ = apiUrl;
}
std::string WeatherApiClient::getWeatherData(double latitude, double longitude, const std::string& date)
{
    std::string url = apiUrl + "?lat=" + std::to_string(latitude) +
                          "&lng=" + std::to_string(longitude) +
                          "&date=" + date + "&formatted=0";

    try {
        cpr::Response response = Get(cpr::Url{url});
        if (response.status_code != 200) {
            throw std::runtime_error("Failed to fetch weather data: " + response.error.message);
        }
        return response.text;
    } catch (const std::exception& ex) {
        std::cerr << "Error fetching weather data: " << ex.what() << '\n';
        throw;
    }
}