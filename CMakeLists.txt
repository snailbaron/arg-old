cmake_minimum_required(VERSION 3.0)

project(arg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

add_library(arg INTERFACE)
target_include_directories(arg INTERFACE include)

add_subdirectory(examples)

add_subdirectory(deps)
add_subdirectory(tests)
