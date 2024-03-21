cmake_minimum_required(VERSION 3.7)

project(drink)
set(CMAKE_C_STANDARD 99)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
)

file(GLOB_RECURSE SOURCE_LIST
        "*.c" "*.h"
)

add_executable(${PROJECT_NAME} ${SOURCE_LIST})