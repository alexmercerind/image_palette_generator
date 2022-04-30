cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "image_palette_generator")
set(VERSION "0.0.1")

project(${PROJECT_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

set(POSITION_INDEPENDENT_CODE ON)

add_library(
    ${PROJECT_NAME} SHARED
    "color.cc"
    "color_count.cc"
    "image_palette_generator.cc"
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    "external/stb"
)

target_link_libraries(
    ${PROJECT_NAME}
    "m"
)
