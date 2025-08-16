## About
A simple [Telegram bot](https://t.me/DayIncreaseBot) that tracks the growing daylight from the winter to the summer solstice. It can be used in group chats, where the location should be sent as a reply to the bot’s message: "To receive the info, please share your location:".

It supports these commands:  
- **/gettodaysinfo** – Get today’s daylight info.  
  <details>
  <summary>Show screenshot</summary>
  
  ![/gettodaysinfo]()
  </details>

- **/getdaystillsolstice** – Show days left until the solstice.  
  <details>
  <summary>Show screenshot</summary>
  
  ![/getdaystillsolstice]()
  </details>

- **/changelocation** – Set or change location.  
  <details>
  <summary>Show screenshot</summary>
  
  ![/changelocation]()
  </details>

- **/setintervals** – Set intervals for receiving info. The interval starts at the time when the command is executed, and the message is sent every 24 hours.  
  <details>
  <summary>Show screenshot</summary>
  
  ![/setintervals]()
  </details>

- **/cancelintervals** – Cancel receiving messages at intervals.  
  <details>
  <summary>Show screenshot</summary>
  
  ![/cancelintervals]()
  </details>

## Dependencies
- [tgbot-cpp](https://github.com/reo7sp/tgbot-cpp) library  
- [Sunrise-Sunset](https://sunrise-sunset.org/api) API for current sunrise and sunset times  
- [TimeZoneDB](https://timezonedb.com/api) API for timezone information  
- [nlohmann_json](https://github.com/nlohmann/json) for parsing  
- [restclient-cpp](https://github.com/mrtazz/restclient-cpp) for HTTP requests  

## Build Tools
- [Docker](https://hub.docker.com/repository/docker/elsandbox/dayincreasebotcpp/tags) (Linux)  
- CMake  

The bot receives incoming updates via long polling.  

Previously deployed in the cloud (Ubuntu). Currently offline while being prepared for deployment on a Raspberry Pi.  
