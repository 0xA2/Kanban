cmake_minimum_required(VERSION 3.17)
project(Kanban_board C)

set(CMAKE_C_STANDARD 11)

find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})
include_directories(.)

add_executable(
        utils/card.c
        utils/card.h
        main.c
        utils/tasklist.c
        utils/tasklist.h utils/render.c utils/render.h)
target_link_libraries(Kanban_board ${CURSES_LIBRARIES})