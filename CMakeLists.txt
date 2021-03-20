cmake_minimum_required(VERSION 3.17)
project(Kanban_board C)

set(CMAKE_C_STANDARD 11)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
include_directories(.)

add_executable(Kanban_board
        card.c
        card.h
        main.c
        tasklist.c
        tasklist.h render.c render.h old_render.c old_render.h core.c core.h read.c read.h)
target_link_libraries(Kanban_board ${CURSES_LIBRARIES})