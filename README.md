A simple [telegram bot](https://t.me/DayIncreaseBot) that tracks the growing daylight from the winter to the summer solstice.

**Dependencies:**
- [tgbot-cpp](https://github.com/reo7sp/tgbot-cpp) library
- [Sunrise-Sunset](https://sunrise-sunset.org/api) API for current sunrise and sunset times
- [TimeZoneDB](https://timezonedb.com/api) API for timezone information
- [nlohmann_json](https://github.com/nlohmann/json) for parsing
- [restclient-cpp](https://github.com/mrtazz/restclient-cpp) for HTTP requests

**Build Tools:**
- Docker
- CMake

The bot receives incoming updates via long polling.

