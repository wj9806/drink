cmake_minimum_required(VERSION 3.7)

project(drink)
set(CMAKE_C_STANDARD 99)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

include_directories(
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/cli
        ${PROJECT_SOURCE_DIR}/include/compile
        ${PROJECT_SOURCE_DIR}/include/object
        ${PROJECT_SOURCE_DIR}/include/parser
        ${PROJECT_SOURCE_DIR}/include/tools
        ${PROJECT_SOURCE_DIR}/include/vm
)

file(GLOB_RECURSE SOURCE_LIST
        "cli/*.c"
        "compile/*.c"
        "parser/*.c"
        "tools/*.c"
        "vm/*.c"
        "object/*.c"
        "include/token.list"
)

add_executable(${PROJECT_NAME} ${SOURCE_LIST})