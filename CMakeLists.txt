cmake_minimum_required(VERSION 2.8)
project(judge-server)
LINK_LIBRARIES("pthread")
# LINK_LIBRARIES("seccomp")
set(CMAKE_C_FLAGS "-O3 -std=c++11 -pie -fPIC -pthread")
add_executable(
        judge-server
        src/connector/connector.cpp
        src/connector/connector.h
        src/type/types.cpp
        src/type/types.h
        src/judge/judge.h
        src/judge/judge.cpp
        src/headfile.h
        src/main.cpp
)