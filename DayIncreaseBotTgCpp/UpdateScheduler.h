#ifndef UPDATESCHEDULER_H
#define UPDATESCHEDULER_H
#include <thread>
#include <tgbot/Bot.h>
#include <atomic>

#include "WeatherApiManager.h"
#include "WeatherDataParser.h"

struct SolsticeStatus
{
    bool isSolsticeDay;
    std::string solsticeType;
    bool isDaylightIncreasing;
};

class UpdateScheduler : public std::enable_shared_from_this<UpdateScheduler>
{
public:
    UpdateScheduler(const std::shared_ptr<TgBot::Bot>& bot, const std::shared_ptr<WeatherApiManager>& weatherApiManager,
                    const std::shared_ptr<WeatherDataParser>& weatherDataParser);
    void scheduleUvUpdates(const std::function<void()>& handleDaylightInfo);
    void sendDailyMessage(int64_t chatId);
    static SolsticeStatus getSolsticeStatus(const std::chrono::system_clock::time_point& currentDate);
    void handleDaysTillSolstice(int64_t chatId);
    void cancelUvUpdates();
    int calculateDaysTillNearestSolstice(std::chrono::system_clock::time_point today) const;
    bool getIsDaylightIncreasing() const { return m_isDaylightIncreasing; }

private:
    const std::shared_ptr<TgBot::Bot> m_bot;
    std::shared_ptr<WeatherApiManager> m_weatherApiManager;
    std::shared_ptr<WeatherDataParser> m_weatherDataParser;
    std::thread m_timerThread;
    std::atomic<bool> m_stopTimer = false;
    bool m_isDaylightIncreasing = false;
};

#endif
