cmake_minimum_required(VERSION 3.20)
project(PrismLink C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(prismlink
    src/color_map.c
    src/protocol.c
)

target_include_directories(prismlink PUBLIC include)