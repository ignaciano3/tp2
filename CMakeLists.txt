cmake_minimum_required(VERSION 3.16)
project(heap C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(heap
        heap.h
        testing.c
        testing.h heap.c
        main.c pruebas_heap.c)
