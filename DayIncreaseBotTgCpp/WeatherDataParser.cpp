#include "WeatherDataParser.h"

#include <iostream>
#include <optional>
#include <sstream>
#include <ctime>
#include "SolsticeData.h"

namespace
{
    std::time_t my_timegm(std::tm* tm)
    {
#ifdef _WIN64
        return _mkgmtime(tm);
#else
        return timegm(tm);
#endif
    }
}

std::string WeatherDataParser::parseTzId(const std::string& apiResponse) {

    try {
        nlohmann::json response = nlohmann::json::parse(apiResponse);
        if (!response.contains("zoneName"))
        {
            return "Error parsing time zone data";
        }

        std::string zoneName = response["zoneName"];
        return zoneName;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error parsing time zone data: " << ex.what() << '\n';
        return "Error parsing time zone data";
    }
}

std::string WeatherDataParser::parseSunriseTime(const std::string& apiResponse)
{
    try
    {
        nlohmann::json jsonResult = nlohmann::json::parse(apiResponse);

        if (!jsonResult.contains("results") || !jsonResult["results"].contains("sunrise"))
        {
            return "Error parsing sunrise time";
        }

        std::string sunrise = jsonResult["results"]["sunrise"];

        std::tm tm = {};
        std::istringstream ss(sunrise);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        if (ss.fail())
        {
            return "Error parsing sunrise time";
        }
        std::ostringstream output;
        output << std::put_time(&tm, "%H:%M:%S");

        return output.str();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error parsing sunrise time: " << ex.what() << '\n';
        return "Error parsing sunrise time";
    }
}

std::string WeatherDataParser::parseSunsetTime(const std::string& apiResponse)
{
    try
    {
        nlohmann::json jsonResult = nlohmann::json::parse(apiResponse);

        if (!jsonResult.contains("results") || !jsonResult["results"].contains("sunset"))
        {
            return "Error parsing sunset time";
        }

        std::string sunset = jsonResult["results"]["sunset"];

        std::tm tm = {};
        std::istringstream ss(sunset);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        if (ss.fail())
        {
            return "Error parsing sunset time";
        }

        std::ostringstream output;
        output << std::put_time(&tm, "%H:%M:%S");

        return output.str();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error parsing sunset time: " << ex.what() << '\n';
        return "Error parsing sunset time";
    }
}

std::string WeatherDataParser::parseDayLength(const std::string& apiResponseToday,
                                              const std::string& apiResponseYesterday,
                                              const std::string& apiResponseShortestDay)
{
    try
    {
        nlohmann::json jsonResultToday = nlohmann::json::parse(apiResponseToday);
        nlohmann::json jsonResultYesterday = nlohmann::json::parse(apiResponseYesterday);
        nlohmann::json jsonResultShortestDay = nlohmann::json::parse(apiResponseShortestDay);

        const std::optional<long> dayLengthSecondsToday =
            jsonResultToday["results"].find("day_length") != jsonResultToday["results"].end()
                ? std::optional(jsonResultToday["results"]["day_length"].get<long>())
                : std::nullopt;
        const std::optional<long> dayLengthSecondsYesterday = jsonResultYesterday["results"].find("day_length") !=
                                                        jsonResultYesterday["results"].end()
                                                            ? std::optional(
                                                                jsonResultYesterday["results"]["day_length"].get<
                                                                    long>())
                                                            : std::nullopt;
        const std::optional<long> dayLengthSecondsShortestDay = jsonResultShortestDay["results"].find("day_length") !=
                                                          jsonResultShortestDay["results"].end()
                                                              ? std::optional(
                                                                  jsonResultShortestDay["results"]["day_length"].get<
                                                                      long>())
                                                              : std::nullopt;

        if (dayLengthSecondsToday && dayLengthSecondsYesterday && dayLengthSecondsShortestDay)
        {
            return calculateDayLength(dayLengthSecondsToday.value(), dayLengthSecondsYesterday.value(),
                                      dayLengthSecondsShortestDay.value());
        }

        return "Error: dayLengthSeconds is null";
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error parsing day length: " << ex.what() << '\n';
        return "Error: exception caught";
    }
}

namespace
{
    std::string formatDuration(const std::chrono::seconds duration)
    {
        const auto h = std::chrono::duration_cast<std::chrono::hours>(duration);
        const auto m = std::chrono::duration_cast<std::chrono::minutes>(duration - h);
        const auto s = std::chrono::duration_cast<std::chrono::seconds>(duration - h - m);

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << h.count() << ":"
            << std::setw(2) << std::setfill('0') << m.count() << ":"
            << std::setw(2) << std::setfill('0') << s.count();

        return oss.str();
    }
}

std::string WeatherDataParser::calculateDayLength(const long todayLength, const long yesterdayLength, const long shortestDayLength)
{
    const std::chrono::seconds dayLengthToday(todayLength);
    const std::chrono::seconds dayLengthDifference(todayLength - yesterdayLength);
    const std::chrono::seconds shortestDayDifference(todayLength - shortestDayLength);

    std::string formattedDayLength = formatDuration(dayLengthToday) +
        "\nThe difference between yesterday and today: " + formatDuration(dayLengthDifference) +
        "\nThe difference between today and the shortest day: " + formatDuration(shortestDayDifference);

    return formattedDayLength;
}
