cmake_minimum_required(VERSION 3.16)
project(SeaTradeSimulatorLab05 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(lab05
    src/main.cpp
    src/Demo.cpp
    src/Game.cpp
    src/Ui.cpp
    src/Port.cpp
    src/BaseShip.cpp
    src/TradeSloop.cpp
    src/NavyFrigate.cpp
    src/Galleon.cpp
    src/CoastalBattery.cpp
    src/StormTactics.cpp
    src/Captain.cpp
    src/VoyageLog.cpp
    src/Operations.cpp
)

target_include_directories(lab05 PRIVATE include)

if(MSVC)
    target_compile_options(lab05 PRIVATE /W4 /utf-8)
else()
    target_compile_options(lab05 PRIVATE -Wall -Wextra -Wpedantic)
endif()
