cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME SpecPainterCore)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

project(${PROJECT_NAME})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -g -ggdb -ffast-math")

set(SOURCE_FILES src/main.c src/utils/png/png.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
