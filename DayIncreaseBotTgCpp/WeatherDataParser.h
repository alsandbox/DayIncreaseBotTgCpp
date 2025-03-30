#ifndef WEATHERDATAPARSER_H
#define WEATHERDATAPARSER_H
#include <string>
#include <nlohmann/json.hpp>

struct WeatherData
{
    std::string sunriseTime;
    std::string sunsetTime;
    std::string dayLength;

    static WeatherData fromJson(const nlohmann::json& json)
    {
        WeatherData weatherData;
        if (json.contains("SunriseTime"))
        {
            weatherData.sunriseTime = json["SunriseTime"].get<std::string>();
        }
        else
        {
            weatherData.sunriseTime = "N/A";
        }

        if (json.contains("SunsetTime"))
        {
            weatherData.sunsetTime = json["SunsetTime"].get<std::string>();
        }
        else
        {
            weatherData.sunsetTime = "N/A";
        }

        if (json.contains("DayLength"))
        {
            weatherData.dayLength = json["DayLength"].get<std::string>(); 
        }
        else
        {
            weatherData.dayLength = "N/A";
        }

        return weatherData;
    }
};

class WeatherDataParser {
public:
    WeatherDataParser() = default;
    static std::string parseTzId(const std::string& apiResponse);
    static std::string parseSunriseTime(const std::string& apiResponse);
    static std::string parseSunsetTime(const std::string& apiResponse);
    static std::string parseDayLength(const std::string& apiResponseToday, const std::string& apiResponseYesterday,
                                      const std::string& apiResponseShortestDay);
    static std::string calculateDayLength(long todayLength, long yesterdayLength, long shortestDayLength);
};

#endif
