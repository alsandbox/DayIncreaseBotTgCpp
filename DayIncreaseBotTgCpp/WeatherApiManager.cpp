#include "WeatherApiManager.h"
#include <iostream>
#include <string>
#include <chrono>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "SolsticeData.h"
#include "WeatherApiClient.h"
#include "WeatherDataParser.h"

WeatherApiManager::WeatherApiManager(std::shared_ptr<WeatherApiClient> const& weatherApiClient)
    : weatherApiClient_(weatherApiClient){}

std::string WeatherApiManager::getTimeAsync(std::chrono::system_clock::time_point date)
{
    constexpr unsigned int day_hours = 24;
    std::chrono::time_point<std::chrono::system_clock> yesterday = date - std::chrono::hours(day_hours);

    int year = SolsticeData::getYearFromDate(date);
    auto solstice = SolsticeData::getSolsticeByYear(year);

    if (!solstice)
    {
        throw std::invalid_argument("Argument 'solstice' cannot be null.");
    }
    
    std::chrono::system_clock::time_point winterSolstice = solstice->first;
    try {
        std::string formattedDate = SolsticeData::formatDate(date);
        std::string formattedYesterday = SolsticeData::formatDate(yesterday);
        std::string formattedWinterSolstice = SolsticeData::formatDate(winterSolstice);

        std::string resultToday = weatherApiClient_->getWeatherData(GetLatitude(), GetLongitude(), formattedDate);
        std::string resultYesterday = weatherApiClient_->getWeatherData(GetLatitude(), GetLongitude(),  formattedYesterday);
        std::string resultShortestDay = weatherApiClient_->getWeatherData(GetLatitude(), GetLongitude(), formattedWinterSolstice);

        std::string sunriseTime = WeatherDataParser::ParseSunriseTime(resultToday);
        std::string sunsetTime = WeatherDataParser::ParseSunsetTime(resultToday);
        std::string dayLength = WeatherDataParser::ParseDayLength(resultToday, resultYesterday, resultShortestDay);

        nlohmann::json weatherInfo = {
            {"SunriseTime", sunriseTime},
            {"SunsetTime", sunsetTime},
            {"DayLength", dayLength}
        };

        return weatherInfo.dump();
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        nlohmann::json errorResponse = {{"Error", "Failed to fetch weather data"}};
        return errorResponse.dump();
    }
}
