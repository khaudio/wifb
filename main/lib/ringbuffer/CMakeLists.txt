cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(ringbuffer src/ringbuffer.cpp)

target_include_directories(ringbuffer
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
