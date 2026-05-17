cmake_minimum_required(VERSION 3.10)
project(BOX42 C)

set(CMAKE_C_STANDARD 99)

find_package(SQLite3 REQUIRED)

# WICHTIG: von BOX42/bin/ eine Ebene hoch
include_directories(
    ../src
    ../src/core
)

add_executable(BOX42
    ../src/core/main.c
    ../src/core/box42.c
    ../src/core/box42_init.c
    ../src/core/box42_os.c
    ../src/core/box42_strds.c
    ../src/core/config.c
    ../src/core/session.c
    ../src/core/mail.c
    ../src/core/sha256.c
    ../src/core/hystack.c
    ../src/core/ax25.c
    ../src/core/ax25_drv.c
)

target_link_libraries(BOX42
    SQLite::SQLite3
)
