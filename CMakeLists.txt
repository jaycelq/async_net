cmake_minimum_required(VERSION 2.8.0)
project(async_event_net CXX)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

FILE(GLOB_RECURSE SOURCE_H include/*.h)
FILE(GLOB_RECURSE SOURCES src/*.cpp)

set(SOURCES ${SOURCE_H} ${SOURCES})

add_library(asyc_event_net ${SOURCES})

add_executable(main main.cpp)
target_link_libraries(main asyc_event_net)
