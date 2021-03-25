cmake_minimum_required(VERSION 3.17)
project(Kanban_board C)

set(CMAKE_C_STANDARD 11)

find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})
include_directories(.)
include_directories(./utils)

add_executable(kanban
./utils/card.c
./utils/card.h
./utils/tasklist.c
./utils/tasklist.h
./utils/render.c
./utils/render.h
main.c utils/core.c utils/core.h)

target_link_libraries(kanban ${CURSES_LIBRARIES})