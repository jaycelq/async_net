cmake_minimum_required(VERSION 3.8)
project(async_event_net)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp include/async_net.h)
add_executable(async_event_net ${SOURCE_FILES})