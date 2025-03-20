#include "UpdateScheduler.h"

#include <iostream>

#include "SolsticeData.h"

UpdateScheduler::UpdateScheduler(const std::shared_ptr<TgBot::Bot>& bot,
                                 const std::shared_ptr<WeatherApiManager>& weatherApiManager,
                                 const std::shared_ptr<WeatherDataParser>& weatherDataParser)
    : bot_(bot),
      weatherApiManager_(weatherApiManager),
      weatherDataParser_(weatherDataParser)
{
}

void UpdateScheduler::scheduleUvUpdates(const std::function<void()>& handleDaylightInfo)
{
    stopTimer = false;
    timerThread = std::thread([self = shared_from_this(), handleDaylightInfo]
    {
        while (!self->stopTimer)
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
        (void)bot_->getApi().sendMessage(chatId, "It's the " + solsticeType + " solstice.", 0);
    }
    
    this->isDaylightIncreasing = isDaylightIncreasingLocal;
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

void UpdateScheduler::handleDaysTillSolstice(int64_t chatId)
{
    std::thread(&UpdateScheduler::sendDailyMessage, this, chatId).detach();

    if (!isDaylightIncreasing)
    {
        const auto today = std::chrono::system_clock::now();

        (void)bot_->getApi().sendMessage(chatId,
                                         "Days till the solstice: " + std::to_string(
                                             WeatherDataParser::calculateDaysTillNearestSolstice(today)), 0);
    }
    else
    {
        (void)bot_->getApi().sendMessage(chatId, "Daylight hours are increasing, wait for the summer solstice.",
                                         0);
    }
}

void UpdateScheduler::cancelUvUpdates()
{
    stopTimer = true;
    if (timerThread.joinable())
    {
        timerThread.join();
    }
}
