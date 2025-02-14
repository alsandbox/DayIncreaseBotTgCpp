#include "LocationService.h"

LocationService::LocationService(const std::shared_ptr<TgBot::Bot> bot, const std::shared_ptr<WeatherApiManager> & weather_api_manager)
    : bot_(bot),
      weather_api_manager_(weather_api_manager){}

void LocationService::RequestLocation(long chat_id) const
{
    const TgBot::ReplyKeyboardMarkup::Ptr keyboard(new TgBot::ReplyKeyboardMarkup);
    
    TgBot::KeyboardButton::Ptr const location_button(new TgBot::KeyboardButton);
    location_button->text = "Send Location";
    location_button->requestLocation = true; 
    
    std::vector<TgBot::KeyboardButton::Ptr> row;
    row.push_back(location_button);

    keyboard->keyboard.push_back(row);

    keyboard->resizeKeyboard = true;
    keyboard->oneTimeKeyboard = true;

    try {
    (void)bot_->getApi().sendMessage(chat_id, "To receive the info, please share your location:", false, 0, keyboard);
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << '\n';
    }
}

void LocationService::HandleLocationReceived(long chat_id, std::atomic<bool>& cancellation_token, const TgBot::Message::Ptr& message)
{
    const std::shared_ptr<TgBot::Location> location = message->location;
    
    if (location == nullptr || (location->latitude <= 0 && location->longitude <= 0))
    {
        try {
            (void)bot_->getApi().sendMessage(chat_id, "Invalid location received. Please try again.", false, 0);
        } catch (const std::exception& e) {
            std::cerr << "Error sending message: " << e.what() << '\n';
        }

        RequestLocation(chat_id);
    }

    is_location_received = true;
    
    weather_api_manager_->SetLatitude(location->latitude);
    
    try {
        (void)bot_->getApi().sendMessage(chat_id, "Location received. You can now start receiving information.", false, 0);
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << '\n';
    }

    TgBot::ReplyKeyboardRemove::Ptr removeKeyboard(new TgBot::ReplyKeyboardRemove);
    (void)bot_->getApi().sendMessage(chat_id, 
        "Location received. You can now start receiving information.", 
        false,
        0,
        removeKeyboard
    );

    if (on_location_received) { 
        auto callback = std::move(on_location_received);
        on_location_received = nullptr; 
        callback();
    }
}
