link_libraries(arg Catch2::Catch2)

add_executable(basic basic.cpp)
add_test(NAME basic COMMAND basic)
