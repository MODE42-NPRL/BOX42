cmake_minimum_required(VERSION 3.10)
project(BOX42 C)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(SOURCES
    ${SRC_DIR}/core/main.c
    ${SRC_DIR}/core/coreloop.c
    ${SRC_DIR}/core/listener.c
    ${SRC_DIR}/core/session.c
    ${SRC_DIR}/core/commands.c
    ${SRC_DIR}/core/config.c
    ${SRC_DIR}/core/help.c
    ${SRC_DIR}/core/login.c
    ${SRC_DIR}/core/mail.c
    ${SRC_DIR}/core/mail_start.c
    ${SRC_DIR}/core/chat.c
    ${SRC_DIR}/core/chat_action.c
    ${SRC_DIR}/core/chat_commands.c
    ${SRC_DIR}/core/chat_parser.c
    ${SRC_DIR}/core/chat_private.c
    ${SRC_DIR}/core/banner.c
    ${SRC_DIR}/core/beacon.c
    ${SRC_DIR}/core/digipeater.c
    ${SRC_DIR}/core/encoding.c
    ${SRC_DIR}/core/events.c
    ${SRC_DIR}/core/event_loop.c
    ${SRC_DIR}/core/focus.c
    ${SRC_DIR}/core/box.c
    ${SRC_DIR}/core/bootstrap.c

    ${SRC_DIR}/arch/x86_64/arch_x86_64.c

    ${SRC_DIR}/core/ax25.c
    ${SRC_DIR}/core/ax25_mod.c
    ${SRC_DIR}/core/ax25_route.c
    ${SRC_DIR}/core/ax25_sniffer.c
    ${SRC_DIR}/core/ax25_tunnel.c
    ${SRC_DIR}/core/kiss.c
)

add_executable(box42 ${SOURCES})

target_include_directories(box42 PRIVATE
    ${SRC_DIR}
    ${SRC_DIR}/core
    ${SRC_DIR}/arch
)

