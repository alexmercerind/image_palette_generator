cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "image_palette_generator")
set(VERSION "0.0.1")

project(${PROJECT_NAME} LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)

add_library(
    ${PROJECT_NAME} STATIC
    "color.cc"
    "color_count.cc"
    "image_palette_generator.cc"
    "external/exoquant/exoquant.c"
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    "external/stb"
    "external/exoquant"
)
