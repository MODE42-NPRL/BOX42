cmake_minimum_required(VERSION 3.10)
project(box42 C)

set(CMAKE_C_STANDARD 11)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../session
    ${CMAKE_CURRENT_SOURCE_DIR}/../util
    ${CMAKE_CURRENT_SOURCE_DIR}/../crypto
    ${CMAKE_CURRENT_SOURCE_DIR}/../net
    ${CMAKE_CURRENT_SOURCE_DIR}/../os
    ${CMAKE_CURRENT_SOURCE_DIR}/../storage
    ${CMAKE_CURRENT_SOURCE_DIR}/../cmd
    ${CMAKE_CURRENT_SOURCE_DIR}/../subsystems
)

file(GLOB_RECURSE BOX42_SOURCES
    "../core/*.c"
    "../session/*.c"
    "../util/*.c"
    "../crypto/*.c"
    "../net/**/*.c"
    "../os/**/*.c"
    "../storage/*.c"
    "../cmd/*.c"
    "../subsystems/*.c"
)

add_executable(box42 ${BOX42_SOURCES})
