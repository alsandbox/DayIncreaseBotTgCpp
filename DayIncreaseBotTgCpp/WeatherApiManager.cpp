#include "WeatherApiManager.h"

#include <iostream>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "SolsticeData.h"
#include "WeatherApiClient.h"
#include "WeatherDataParser.h"

WeatherApiManager::WeatherApiManager(const std::shared_ptr<WeatherApiClient>& weatherApiClient)
    : weatherApiClient_(weatherApiClient)
{
}

std::string WeatherApiManager::getParsedTzId(const double latitude, const double longitude) const
{
    const std::string resultTzId = weatherApiClient_->getTzId(latitude, longitude);
    std::string parsedTzId = WeatherDataParser::parseTzId(resultTzId);
    return parsedTzId;
}

std::string WeatherApiManager::getTime(std::chrono::system_clock::time_point date) const
{
    constexpr int day_hours = 24;
    auto yesterday = date - std::chrono::hours(day_hours);

    int year = SolsticeData::getYearFromDate(date);
    auto solstice = SolsticeData::getSolsticeByYear(year);

    if (!solstice.has_value())
    {
        throw std::invalid_argument("Argument 'solstice' cannot be null.");
    }

    std::chrono::system_clock::time_point winterSolstice = solstice.value().first;
    try
    {
        std::string formattedDate = SolsticeData::formatDate(date);
        std::string formattedYesterday = SolsticeData::formatDate(yesterday);
        std::string formattedWinterSolstice = SolsticeData::formatDate(winterSolstice);

        std::string tzId = getParsedTzId(latitude, longitude);
        std::string resultToday = weatherApiClient_->getWeatherData(getLatitude(), getLongitude(), formattedDate, tzId);
        std::string resultYesterday = weatherApiClient_->getWeatherData(getLatitude(), getLongitude(),
                                                                        formattedYesterday, tzId);
        std::string resultShortestDay = weatherApiClient_->getWeatherData(
            getLatitude(), getLongitude(), formattedWinterSolstice, tzId);

        std::string sunriseTime = WeatherDataParser::parseSunriseTime(resultToday);
        std::string sunsetTime = WeatherDataParser::parseSunsetTime(resultToday);
        std::string dayLength = WeatherDataParser::parseDayLength(resultToday, resultYesterday, resultShortestDay);

        nlohmann::json weatherInfo = {
            {"SunriseTime", sunriseTime},
            {"SunsetTime", sunsetTime},
            {"DayLength", dayLength}
        };

        return weatherInfo.dump(3);
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << '\n';
        nlohmann::json errorResponse = {{"Error", "Failed to fetch weather data"}};
        return errorResponse.dump();
    }
}
