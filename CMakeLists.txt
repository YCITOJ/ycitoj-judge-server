cmake_minimum_required(VERSION 3.10)
project(JudgeServerV2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
LINK_LIBRARIES("pthread")
add_executable(
    JudgeServerV2 
    ./src/main.cpp
    ./src/compare.cpp
    ./src/definitions.cpp
    ./src/file.cpp
    ./src/GUI.cpp
    ./src/json_parser.cpp
    ./src/json_parser.hpp
    ./src/judge.cpp
    ./src/message.cpp
    ./src/net.cpp
    ./src/utile.cpp
    )
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")
install(TARGETS JudgeServerV2 RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})