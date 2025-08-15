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
    bool getIsDaylightIncreasing() const { return isDaylightIncreasing; }

private:
    const std::shared_ptr<TgBot::Bot> bot_;
    std::shared_ptr<WeatherApiManager> weatherApiManager_;
    std::shared_ptr<WeatherDataParser> weatherDataParser_;
    std::thread timerThread;
    std::atomic<bool> stopTimer = false;
    bool isDaylightIncreasing = false;
};

#endif
