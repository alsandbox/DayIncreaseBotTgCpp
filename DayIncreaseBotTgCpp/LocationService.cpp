#include "LocationService.h"

LocationService::LocationService(const std::shared_ptr<TgBot::Bot> bot, const std::shared_ptr<WeatherApiManager> & weatherApiManager)
    : bot_(bot),
      weatherApiManager_(weatherApiManager){}

void LocationService::requestLocation(long chatId) const
{
    const TgBot::ReplyKeyboardMarkup::Ptr keyboard(new TgBot::ReplyKeyboardMarkup);
    
    TgBot::KeyboardButton::Ptr const locationButton(new TgBot::KeyboardButton);
    locationButton->text = "Send Location";
    locationButton->requestLocation = true; 
    
    std::vector<TgBot::KeyboardButton::Ptr> row;
    row.push_back(locationButton);

    keyboard->keyboard.push_back(row);

    keyboard->resizeKeyboard = true;
    keyboard->oneTimeKeyboard = true;

    try {
    (void)bot_->getApi().sendMessage(chatId, "To receive the info, please share your location:", false, 0, keyboard);
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << '\n';
    }
}

void LocationService::handleLocationReceived(long chatId, std::atomic<bool>& cancellation_token, const TgBot::Message::Ptr& message)
{
    const std::shared_ptr<TgBot::Location> location = message->location;
    
    if (location == nullptr || (location->latitude <= 0 && location->longitude <= 0))
    {
        try {
            (void)bot_->getApi().sendMessage(chatId, "Invalid location received. Please try again.", false, 0);
        } catch (const std::exception& e) {
            std::cerr << "Error sending message: " << e.what() << '\n';
        }

        requestLocation(chatId);
    }

    isLocationReceived = true;

    weatherApiManager_->setLatitude(location->latitude);
    weatherApiManager_->setLongitude(location->longitude);
    
    try {
        (void)bot_->getApi().sendMessage(chatId, "Location received. You can now start receiving information.", false, 0);
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << '\n';
    }

    TgBot::ReplyKeyboardRemove::Ptr removeKeyboard(new TgBot::ReplyKeyboardRemove);
    (void)bot_->getApi().sendMessage(chatId, 
        "Location received. You can now start receiving information.", 
        false,
        0,
        removeKeyboard
    );

    if (onLocationReceived) { 
        const auto callback = std::move(onLocationReceived);
        onLocationReceived = nullptr; 
        callback();
    }
}
