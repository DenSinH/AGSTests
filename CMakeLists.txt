cmake_minimum_required(VERSION 3.17)
project(AGS C)

set(CMAKE_C_STANDARD 11)

add_library(
        AGS
        default.h
        memory/memory.c
        memory/memory.h
        AGS.c AGS.h)

include_directories(AGS ${CMAKE_CURRENT_SOURCE_DIR})