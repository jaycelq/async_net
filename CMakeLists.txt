cmake_minimum_required(VERSION 2.8.0)
project(async_event_net CXX)

set(CMAKE_CXX_STANDARD 11)
string(LENGTH "${CMAKE_SOURCE_DIR}/" SOURCE_PATH_SIZE)
add_definitions("-DSOURCE_PATH_SIZE=${SOURCE_PATH_SIZE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")
include_directories(include)
include_directories(../../3rd_party/boost_1_64_0/include)

link_directories(${CMAKE_SOURCE_DIR}/../../3rd_party/boost_1_64_0/lib/)

FILE(GLOB_RECURSE SOURCE_H include/*.h)
FILE(GLOB_RECURSE SOURCES src/*.cpp)

set(SOURCES ${SOURCE_H} ${SOURCES})

add_library(asyc_event_net ${SOURCES})

add_executable(main main.cpp)
target_link_libraries(main asyc_event_net libboost_system.a)

add_executable(demo demo.cpp)
target_link_libraries(demo asyc_event_net libboost_system.a)
