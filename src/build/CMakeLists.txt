cmake_minimum_required(VERSION 3.5)
project(BOX42 C)

set(CMAKE_C_STANDARD 99)

set(BOX42_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

include_directories(
    ${BOX42_ROOT}/src
    ${BOX42_ROOT}/src/core
    ${BOX42_ROOT}/src/arch
    ${BOX42_ROOT}/src/net
    ${BOX42_ROOT}/src/os
    ${BOX42_ROOT}/src/session
    ${BOX42_ROOT}/src/storage
    ${BOX42_ROOT}/src/util
)

file(GLOB CORE_SRC
    "${BOX42_ROOT}/src/core/*.c"
    "${BOX42_ROOT}/src/session/*.c"
    "${BOX42_ROOT}/src/net/telnet/*.c"
    "${BOX42_ROOT}/src/storage/storage*.c"
    "${BOX42_ROOT}/src/os/os_*.c"
)

add_executable(box42 ${CORE_SRC})
set_target_properties(box42 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${BOX42_ROOT}/bin")
