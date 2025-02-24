#ifndef SOLSTICEDATA_H
#define SOLSTICEDATA_H
#include <chrono>
#include <map>
#include <optional>

class SolsticeData
{
public:
    SolsticeData() = default;
    static std::map<int, std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>> solsticeData;
    static std::optional<std::pair<std::chrono::system_clock::time_point, std::chrono::system_clock::time_point>> getSolsticeByYear(int year);
    static int getYearFromDate(const std::chrono::system_clock::time_point& date);
    static std::string formatDate(const std::chrono::system_clock::time_point& date);
};

#endif