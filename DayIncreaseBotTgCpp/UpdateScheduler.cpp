#include "UpdateScheduler.h"

#include <iostream>

#include "SolsticeData.h"

UpdateScheduler::UpdateScheduler(const std::shared_ptr<TgBot::Bot>& bot,
                                 const std::shared_ptr<WeatherApiManager>& weatherApiManager,
                                 const std::shared_ptr<WeatherDataParser>& weatherDataParser)
    : m_bot(bot),
      m_weatherApiManager(weatherApiManager),
      m_weatherDataParser(weatherDataParser)
{
}

void UpdateScheduler::scheduleUvUpdates(const std::function<void()>& handleDaylightInfo)
{
    m_stopTimer = false;
    m_timerThread = std::thread([self = shared_from_this(), handleDaylightInfo]
    {
        while (!self->m_stopTimer)
        {
            try
            {
                handleDaylightInfo();
            }
            catch (const std::exception& ex)
            {
                std::cerr << "Error in timer callback: " << ex.what() << '\n';
            }
            std::this_thread::sleep_for(std::chrono::hours(24));
        }
    });
}

void UpdateScheduler::sendDailyMessage(int64_t chatId)
{
    const auto today = std::chrono::system_clock::now();
    std::time_t today_time = std::chrono::system_clock::to_time_t(today);
    std::tm today_tm = {};

#if defined(_WIN32) || defined(_WIN64)
    if (const errno_t err = localtime_s(&today_tm, &today_time); err != 0)
    {
        std::cerr << "Error converting time: " << err << '\n';
    }
#else
    localtime_r(&today_time, &today_tm);
#endif

    std::time_t raw_time = std::mktime(&today_tm);
    std::chrono::system_clock::time_point today_point = std::chrono::system_clock::from_time_t(raw_time);

    auto [isSolsticeDay, solsticeType, isDaylightIncreasingLocal] = getSolsticeStatus(today_point);

    if (isSolsticeDay)
    {
        (void)m_bot->getApi().sendMessage(chatId, "It's the " + solsticeType + " solstice.", nullptr);
    }
    
    this->m_isDaylightIncreasing = isDaylightIncreasingLocal;
}

SolsticeStatus UpdateScheduler::getSolsticeStatus(const std::chrono::system_clock::time_point& currentDate)
{
    int currentYear = SolsticeData::getYearFromDate(currentDate);
    auto solstice = SolsticeData::getSolsticeByYear(currentYear);

    if (!solstice.has_value()) {
        return {false, "", false};
    }

    auto [winterSolstice, summerSolstice] = solstice.value();

    if (currentDate < summerSolstice) {
        auto prevYearSolstice = SolsticeData::getSolsticeByYear(currentYear - 1);
        if (prevYearSolstice.has_value()) {
            winterSolstice = prevYearSolstice.value().first;
        }
    } 
    else if (currentDate > winterSolstice) {
        auto nextYearSolstice = SolsticeData::getSolsticeByYear(currentYear + 1);
        if (nextYearSolstice.has_value()) {
            summerSolstice = nextYearSolstice.value().second;
        }
    }

    bool isSolsticeDay = (currentDate == winterSolstice || currentDate == summerSolstice);

    std::string solsticeType = (isSolsticeDay) ? (currentDate == winterSolstice ? "winter" : "summer") : "";

    bool isDaylightIncreasing = (currentDate > winterSolstice && currentDate < summerSolstice);

    return {isSolsticeDay, solsticeType, isDaylightIncreasing};
}

int UpdateScheduler::calculateDaysTillNearestSolstice(const std::chrono::system_clock::time_point today) const {
    const int year = SolsticeData::getYearFromDate(today);
    auto solsticeOpt= SolsticeData::getSolsticeByYear(year);

    if (!solsticeOpt)
    {
        std::cerr << "Solstice data for the specified year is unavailable.\n";
        return -1;
    }

    auto [winter, summer] = *solsticeOpt;
    std::chrono::system_clock::time_point targetDate;

    if (m_isDaylightIncreasing)
    {
        targetDate = summer;
    }
    else
    {
        targetDate = winter;
    }

    using days = std::chrono::duration<int, std::ratio<86400>>;
    const auto duration = std::chrono::duration_cast<days>(targetDate - today);
    return duration.count();
}

void UpdateScheduler::handleDaysTillSolstice(int64_t chatId)
{
    std::thread(&UpdateScheduler::sendDailyMessage, this, chatId).detach();
    const auto today = std::chrono::system_clock::now();
    sendDailyMessage(chatId);

    if (m_isDaylightIncreasing)
    {
        (void)m_bot->getApi().sendMessage(chatId,
                                         "Days till the summer solstice: " + std::to_string(
                                             calculateDaysTillNearestSolstice(today)), nullptr);
    }
    else
    {
        (void)m_bot->getApi().sendMessage(chatId,
                                         "Days till the winter solstice: " + std::to_string(
                                             calculateDaysTillNearestSolstice(today)), nullptr);
    }
}

void UpdateScheduler::cancelUvUpdates()
{
    m_stopTimer = true;
    if (m_timerThread.joinable())
    {
        m_timerThread.join();
    }
}
