cmake_minimum_required(VERSION 3.17)
project(AGS C)

set(CMAKE_C_STANDARD 11)

add_library(
        AGS
        default.h
        memory/memory.c
        memory/memory.h
        AGS.c AGS.h memory/tests.c lcd/lcd.c lcd/lcd.h lcd/tests.c timer/timer.c timer/timer.h timer/tests.c dma/dma.c dma/dma.h dma/tests.c dma/expected.h)

include_directories(AGS ${CMAKE_CURRENT_SOURCE_DIR})