#include "WeatherApiClient.h"
#include <iostream>
#include <restclient-cpp/restclient.h>

WeatherApiClient::WeatherApiClient(const std::string& apiUrl)
{
    if (apiUrl.empty())
    {
        throw std::invalid_argument("API URL not configured");
    }
    apiUrl_ = apiUrl;
}

std::string WeatherApiClient::getWeatherData(const double latitude, const double longitude, const std::string& date) const
{
    const std::string url = apiUrl_ + "?lat=" + std::to_string(latitude) +
        "&lng=" + std::to_string(longitude) +
        "&date=" + date + "&formatted=0";
    try
    {
        RestClient::Response response = RestClient::get(url);
        if (response.code != 200)
        {
            throw std::runtime_error("Failed to fetch weather data: " + std::to_string(response.code));
        }
        return response.body;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error fetching weather data: " << ex.what() << '\n';
        throw;
    }
    
}
