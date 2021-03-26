cmake_minimum_required(VERSION 3.17)
project(Kanban_board C)

find_package(Curses REQUIRED)
set(CURSES_NEED_WIDE TRUE)
set(CMAKE_C_STANDARD 11)

include_directories(${CURSES_INCLUDE_DIR})
include_directories(.)
include_directories(./utils)

add_executable(kanban

        ./utils/card.h
        ./utils/tasklist.h
        ./utils/render.h
        ./utils/core.h

        ./utils/core.c
        ./utils/card.c
        ./utils/tasklist.c
        ./utils/render.c

        ./main.c
        )

target_link_libraries(kanban ${CURSES_LIBRARIES})