#include "WeatherApiClient.h"
#include <iostream>
#include <restclient-cpp/restclient.h>

WeatherApiClient::WeatherApiClient(const std::string& apiUrl, const std::string& tzApiUrl, const std::string& apiKey)
{
    if (apiUrl.empty())
    {
        throw std::invalid_argument("API URL is not configured");
    }

    if (tzApiUrl.empty()) {
        throw std::invalid_argument("TZ API URL is not configured");
    }

    if (apiKey.empty()) {
        throw std::invalid_argument("API Key is not configured");
    }

    m_apiUrl = apiUrl;
    m_tzApiUrl = tzApiUrl;
    m_apiKey = apiKey;
}
std::string WeatherApiClient::getTzId(const double latitude, const double longitude) const {
    const std::string tzUrl = m_tzApiUrl + "?key=" + m_apiKey + "&format=json&by=position&lat=" + std::to_string(latitude) + "&lng=" + std::to_string(longitude);
    try
    {
        RestClient::Response response = RestClient::get(tzUrl);
        if (response.code != 200)
        {
            throw std::runtime_error("Failed to fetch the time zone data: " + std::to_string(response.code));
        }
        return response.body;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error fetching the time zone data: " << ex.what() << '\n';
        throw;
    }
}

std::string WeatherApiClient::getWeatherData(const double latitude, const double longitude, const std::string& date, const std::string& tzId) const
{
    const std::string url = m_apiUrl + "?lat=" + std::to_string(latitude) +
        "&lng=" + std::to_string(longitude) +
        "&date=" + date + "&formatted=0" + "&tzId=" + tzId;
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
