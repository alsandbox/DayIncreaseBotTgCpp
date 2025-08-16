cmake_minimum_required (VERSION 3.10.2)
project (DayIncreaseBotTgCpp)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED YES)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set (VCPKG_ROOT "/vcpkg")
    set (CMAKE_PREFIX_PATH "${VCPKG_ROOT}/installed/x64-linux")
    set (CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif ()

find_package (Threads REQUIRED)
find_package (OpenSSL REQUIRED)
find_package (Boost REQUIRED COMPONENTS system)

find_package (CURL REQUIRED)

include (FetchContent)
FetchContent_Declare (
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
)
FetchContent_MakeAvailable (nlohmann_json)

include (FetchContent)
FetchContent_Declare (
        tgbot-cpp
        GIT_REPOSITORY https://github.com/reo7sp/tgbot-cpp.git
)
FetchContent_MakeAvailable (tgbot-cpp)
add_library (TgBot::TgBot ALIAS TgBot)

include_directories (${tgbot-cpp_SOURCE_DIR}/include)

FetchContent_Declare (
        restclient-cpp
        GIT_REPOSITORY https://github.com/mrtazz/restclient-cpp.git
        GIT_TAG 0.5.3
)
FetchContent_MakeAvailable (restclient-cpp)

include_directories (${restclient-cpp_SOURCE_DIR}/include)
message (STATUS "RestClient-CPP Source Dir: ${restclient-cpp_SOURCE_DIR}")

add_executable (DayIncreaseBotTgCpp DayIncreaseBotTgCpp.cpp
        WeatherApiClient.cpp
        SolsticeData.cpp
        WeatherApiManager.cpp
        WeatherDataParser.cpp
        BotManager.cpp
        UpdateScheduler.cpp
        MessageHandler.cpp
        LocationService.cpp
        SolsticeData.h
        MessageHandler.h
        LocationService.h
        UpdateScheduler.h
        WeatherDataParser.h
        WeatherApiManager.h
        WeatherApiClient.h
        BotManager.h
)

target_link_libraries (DayIncreaseBotTgCpp
        ${CMAKE_THREAD_LIBS_INIT}
        ${OPENSSL_LIBRARIES}
        Boost::system
        ${CURL_LIBRARY}
        nlohmann_json::nlohmann_json
        TgBot::TgBot
        restclient-cpp
)