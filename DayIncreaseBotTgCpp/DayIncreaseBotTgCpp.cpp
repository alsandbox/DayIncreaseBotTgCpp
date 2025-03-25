#include <tgbot/tgbot.h>
#include <string>
#include "BotManager.h"
#include "SolsticeData.h"
#include "WeatherApiManager.h"

std::atomic<bool> isRunning(true);

void handleSignal(const int signal) {
    std::cout << "Received signal " << signal << ", shutting down...\n";
    isRunning = false;
}

static std::string loadApiUrl()
{
    std::filesystem::path settingsFilePath = std::filesystem::current_path() / "settings.json";
    std::cerr << "Looking for settings.json at: " << settingsFilePath << '\n';

    if (std::ifstream file(settingsFilePath); file.is_open())
    {
        nlohmann::json j;
        file >> j;

        if (j.contains("ApiSettings") && j["ApiSettings"].contains("ApiUrl"))
        {
            return j["ApiSettings"]["ApiUrl"].get<std::string>();
        }
    }
    
    std::cerr << "Failed to load API URL." << '\n';
    return "";
}

int main()
{
    std::signal(SIGTERM, handleSignal);
    std::signal(SIGINT, handleSignal);

    std::string botToken;
    if (std::getenv("BOT_TOKEN"))
        botToken = std::getenv("BOT_TOKEN");
    else
        botToken = "";
    if (botToken.empty()) {
        throw std::runtime_error("BOT_TOKEN not set");
    }
    auto bot = std::make_shared<TgBot::Bot>(botToken);

    std::string apiUrl = loadApiUrl();

    auto weatherApiClient = std::make_shared<WeatherApiClient>(apiUrl);
    auto solsticeData = std::make_shared<SolsticeData>();
    auto weatherApiManager = std::make_shared<WeatherApiManager>(weatherApiClient);
    auto weatherDataParser = std::make_shared<WeatherDataParser>();
    const auto botManager = std::make_shared<BotManager>(bot, weatherApiManager, weatherDataParser, isRunning);

    botManager->startBot();
    
    return 0;
}
