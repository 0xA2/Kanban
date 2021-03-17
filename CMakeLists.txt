cmake_minimum_required(VERSION 3.17)
project(Kanban_board C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Kanban_board
        card.c
        card.h
        main.c
        list.c
        list.h)
