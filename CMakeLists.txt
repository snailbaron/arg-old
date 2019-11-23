cmake_minimum_required(VERSION 3.0)

project(car)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(car
    src/parser.cpp
)
target_include_directories(car PUBLIC include)

add_subdirectory(examples)
