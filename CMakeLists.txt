cmake_minimum_required(VERSION 3.10)

project(read-measurer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include_directories("src")

add_executable(
        ${PROJECT_NAME}
        main.cpp
        src/util.cpp
        src/util.h
)
