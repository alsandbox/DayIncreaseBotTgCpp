#include "SolsticeData.h"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

namespace
{
    std::chrono::system_clock::time_point createDate(int year, int month, int day, int hour, int minute)
    {
        std::tm timeInfo = {};
        timeInfo.tm_year = year - 1900;
        timeInfo.tm_mon = month - 1;
        timeInfo.tm_mday = day;
        timeInfo.tm_hour = hour;
        timeInfo.tm_min = minute;
        timeInfo.tm_sec = 0;

        return std::chrono::system_clock::from_time_t(std::mktime(&timeInfo));
    }
}

std::map<int, std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>> SolsticeData::solsticeData =
{
    {2024, {createDate(2024, 12, 21, 9, 20), createDate(2024, 6, 21, 20, 51)}},
    {2025, {createDate(2025, 12, 21, 15, 3), createDate(2025, 6, 21, 2, 42)}},
    {2026, {createDate(2026, 12, 21, 20, 50), createDate(2026, 6, 21, 8, 25)}},
    {2027, {createDate(2027, 12, 21, 2, 43), createDate(2027, 6, 21, 14, 11)}},
    {2028, {createDate(2028, 12, 21, 8, 20), createDate(2028, 6, 21, 20, 2)}},
    {2029, {createDate(2029, 12, 21, 14, 14), createDate(2029, 6, 21, 1, 48)}},
    {2030, {createDate(2030, 12, 21, 20, 9), createDate(2030, 6, 21, 7, 31)}}
};

std::optional<std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>>
SolsticeData::getSolsticeByYear(const int year)
{
    if (const auto it = solsticeData.find(year); it != solsticeData.end())
    {
        return it->second;
    }

    return std::nullopt;
}

int SolsticeData::getYearFromDate(const std::chrono::system_clock::time_point& date)
{
    std::time_t time = std::chrono::system_clock::to_time_t(date);
    std::tm tmInfo = {};

    gmtime(&time); 
    tmInfo = *gmtime(&time); 

    return tmInfo.tm_year + 1900;
}

std::string SolsticeData::formatDate(const std::chrono::system_clock::time_point& date)
{
    std::time_t time = std::chrono::system_clock::to_time_t(date);
    std::tm tmInfo = {};

    gmtime(&time);
    tmInfo = *gmtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&tmInfo, "%Y-%m-%d");
    return oss.str();
}
