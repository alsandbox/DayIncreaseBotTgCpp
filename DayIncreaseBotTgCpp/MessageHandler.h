#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <memory>
#include <tgbot/tgbot.h>

#include "LocationService.h"
#include "UpdateScheduler.h"
#include "WeatherApiManager.h"


struct StateInfo {
    std::string lastCommand;
    bool hasLocation = false;
};


class MessageHandler {
public:
    MessageHandler(const std::shared_ptr<WeatherApiManager> &weatherApiManager,
                   const std::shared_ptr<LocationService> &locationService, const std::shared_ptr<TgBot::Bot> &bot,
                   const std::shared_ptr<UpdateScheduler> &updateScheduler);

    void handleGetTodayInfo(int64_t chatId) const;

    void handleDaylightInfo(int64_t chatId) const;

    void handleUpdate(int64_t chatId, const TgBot::Message::Ptr &message);

    void scheduleDaylightUpdateWithLambda(int64_t chatId) const;

    void selectCommand(const std::string &command, int64_t chatId, const TgBot::Message::Ptr &message);

    void handleLocation(const TgBot::Message::Ptr &message, int64_t chatId);

    static void handlePollingError(const std::exception &exception);

    void askLocationDependingChatType(const TgBot::Message::Ptr &message, int64_t chatId);

    StateInfo *getStateInfo(const int64_t chatId) {
        const auto it = m_stateInfoDictionary.find(chatId);

        if ((it != m_stateInfoDictionary.end())) {
            return &it->second;
        }

        return nullptr;
    }

    void setStateInfo(const int64_t chatId, const StateInfo &stateInfo) {
        m_stateInfoDictionary[chatId] = stateInfo;
    }

private:
    std::shared_ptr<WeatherApiManager> m_weatherApiManager;
    std::shared_ptr<LocationService> m_locationService;
    std::shared_ptr<UpdateScheduler> m_updateScheduler;
    std::shared_ptr<TgBot::Bot> m_bot;
    bool m_isSentOnce = false;
    bool m_isLocationRequired = false;
    std::unordered_map<int64_t, StateInfo> m_stateInfoDictionary;
};

#endif
