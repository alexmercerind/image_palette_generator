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

if(BUILD_EXAMPLE STREQUAL "ON")
    add_executable(
        "${PROJECT_NAME}_example"
        "example/main.cc"
    )
    target_include_directories(
        "${PROJECT_NAME}_example"
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )
    target_link_libraries(
        "${PROJECT_NAME}_example"
        "$<TARGET_FILE:${PROJECT_NAME}>"
    )
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set_target_properties(
            "${PROJECT_NAME}_example"
            PROPERTIES OUTPUT_NAME "example" SUFFIX ".out"
        )
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set_target_properties(
            "${PROJECT_NAME}_example"
            PROPERTIES OUTPUT_NAME "example" SUFFIX ".out"
        )
    endif()
endif()
