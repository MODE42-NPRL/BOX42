cmake_minimum_required(VERSION 3.10)
project(BOX42 C)

# Quellcode-Verzeichnis
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Alle .c-Dateien rekursiv sammeln
file(GLOB_RECURSE SOURCES
    ${SRC_DIR}/*.c
)

# Executable
add_executable(box42 ${SOURCES})

# Include-Verzeichnisse hinzufügen
target_include_directories(box42 PRIVATE
    ${SRC_DIR}
    ${SRC_DIR}/arch
    ${SRC_DIR}/core
    ${SRC_DIR}/net
    ${SRC_DIR}/os
    ${SRC_DIR}/session
    ${SRC_DIR}/storage
    ${SRC_DIR}/util
)
