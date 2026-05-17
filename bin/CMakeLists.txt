cmake_minimum_required(VERSION 3.10)

project(BOX42 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ------------------------------------------------------------
# BOX42 Core
# ------------------------------------------------------------
set(BOX42_SRC
    ${CMAKE_SOURCE_DIR}/../src/core/box42_init.c
    ${CMAKE_SOURCE_DIR}/../src/core/box42.c
    ${CMAKE_SOURCE_DIR}/../src/core/box42_strds.c
    ${CMAKE_SOURCE_DIR}/../src/core/session.c
    ${CMAKE_SOURCE_DIR}/../src/core/mail.c
    ${CMAKE_SOURCE_DIR}/../src/core/config.c
    ${CMAKE_SOURCE_DIR}/../src/core/sha256.c
    ${CMAKE_SOURCE_DIR}/../src/core/hystack.c
    ${CMAKE_SOURCE_DIR}/../src/core/main.c
)

add_executable(box42
    ${BOX42_SRC}
)

if(UNIX)
    target_link_libraries(box42 PRIVATE m pthread)
endif()
