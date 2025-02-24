#include "SolsticeData.h"

#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>

std::map<int, std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>> SolsticeData::solsticeData =
{
    {2024, {std::chrono::system_clock::from_time_t(0) + std::chrono::hours(20) + std::chrono::minutes(51), std::chrono::system_clock::from_time_t(0) + std::chrono::hours(9) + std::chrono::minutes(20)}},
    {2025, {std::chrono::system_clock::from_time_t(0) + std::chrono::hours(2) + std::chrono::minutes(42), std::chrono::system_clock::from_time_t(0) + std::chrono::hours(15) + std::chrono::minutes(3)}},
    {2026, {std::chrono::system_clock::from_time_t(0) + std::chrono::hours(8) + std::chrono::minutes(25), std::chrono::system_clock::from_time_t(0) + std::chrono::hours(20) + std::chrono::minutes(50)}},
    {2027, {std::chrono::system_clock::from_time_t(0) + std::chrono::hours(14) + std::chrono::minutes(11), std::chrono::system_clock::from_time_t(0) + std::chrono::hours(2) + std::chrono::minutes(43)}},
    {2028, {std::chrono::system_clock::from_time_t(0) + std::chrono::hours(20) + std::chrono::minutes(2), std::chrono::system_clock::from_time_t(0) + std::chrono::hours(8) + std::chrono::minutes(20)}},
    {2029, {std::chrono::system_clock::from_time_t(0) + std::chrono::hours(1) + std::chrono::minutes(48), std::chrono::system_clock::from_time_t(0) + std::chrono::hours(14) + std::chrono::minutes(14)}},
    {2030, {std::chrono::system_clock::from_time_t(0) + std::chrono::hours(7) + std::chrono::minutes(31), std::chrono::system_clock::from_time_t(0) + std::chrono::hours(20) + std::chrono::minutes(9)}}
};

std::optional<std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>> SolsticeData::getSolsticeByYear(int year)
{
    const auto it = solsticeData.find(year);
    
    if (it != solsticeData.end())
    {
        return it->second;
    }

   return std::nullopt;
}

int SolsticeData::getYearFromDate(const std::chrono::system_clock::time_point& date)
{
   std::time_t time = std::chrono::system_clock::to_time_t(date);
   std::tm tmInfo = {};
#ifdef _WIN32
    errno_t err = gmtime_s(&tmInfo, &time); 
    if (err != 0) {
        std::cerr << "Error converting time: " << err << '\n';
        
        return -1;  
    }
#else
    gmtime_r(&time, &tm_info); 
#endif
   return tmInfo.tm_year + 1900;
}

std::string SolsticeData::formatDate(const std::chrono::system_clock::time_point& date) {
    std::time_t time = std::chrono::system_clock::to_time_t(date);
    std::tm tmInfo = {};
        
#ifdef _WIN32
    gmtime_s(&tmInfo, &time);
#else
    gmtime_r(&time, &tmInfo);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tmInfo, "%d-%m-%Y");
    return oss.str();
}

