cmake_minimum_required(VERSION 3.20)

project(SeaTradeSimulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sea_trade_simulator
    src/main.cpp
    src/crew_member.cpp
    src/ship.cpp
    src/trading_company.cpp
)

target_include_directories(sea_trade_simulator
    PRIVATE
        include
)