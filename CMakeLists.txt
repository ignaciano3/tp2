cmake_minimum_required(VERSION 3.21)
project(tp2 C)

set(CMAKE_C_STANDARD 11)

set(PROJECT_HEADERS
        algogram.h
        hash.h
        pila.h
        heap.h
        abb.h
        )

set(PROJECT_SOURCES
        algogram.c
        hash.c
        pila.c
        heap.c
        abb.c
        tp2.c
        )

include_directories(.)

add_executable(tp2 ${PROJECT_SOURCES} ${PROJECT_HEADERS})
