#include "LocationService.h"

LocationService::LocationService(const std::shared_ptr<TgBot::Bot>& bot,
                                 const std::shared_ptr<WeatherApiManager>& weatherApiManager)
    : bot_(bot),
      weatherApiManager_(weatherApiManager)
{
}

void LocationService::requestLocation(int64_t chatId) const {
    try
    {
        auto keyboard = std::make_shared<TgBot::ReplyKeyboardMarkup>();

        auto locationButton = std::make_shared<TgBot::KeyboardButton>();
        locationButton->text = "Send Location";
        locationButton->requestLocation = true;

        std::vector<TgBot::KeyboardButton::Ptr> row;
        row.push_back(locationButton);

        keyboard->keyboard.push_back(row);

        keyboard->resizeKeyboard = true;
        keyboard->oneTimeKeyboard = true;

        (void)bot_->getApi().sendMessage(chatId, "To receive the info, please share your location:", nullptr, nullptr,
                                         keyboard);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error sending message: " << e.what() << '\n';
    }
}

void LocationService::handleLocationReceived(int64_t chatId, const TgBot::Message::Ptr& message)
{
    std::shared_ptr<TgBot::Location> location = message->location;

    if (location == nullptr || (location->latitude <= 0 && location->longitude <= 0))
    {
        try
        {
            (void)bot_->getApi().sendMessage(chatId, "Invalid location received. Please try again.",nullptr);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error sending message: " << e.what() << '\n';
        }

        if (message->chat->type == TgBot::Chat::Type::Private) {
            requestLocation(chatId);
        }
    }

    isLocationReceived = true;

    weatherApiManager_->setLatitude(location->latitude);
    weatherApiManager_->setLongitude(location->longitude);
    
    try
    {
        const TgBot::ReplyKeyboardRemove::Ptr removeKeyboard(new TgBot::ReplyKeyboardRemove);
            (void)bot_->getApi().sendMessage(chatId,
                                             "Location received. You can now start receiving information.",
                                             nullptr,
                                             nullptr,
                                             removeKeyboard
            );
    }
    catch (const std::exception& e)
    {
         std::cerr << "Error sending message: " << e.what() << '\n';
    }

    if (onLocationReceived)
    {
        const auto callback = std::move(onLocationReceived);
        onLocationReceived = nullptr;
        callback();
    }
}
