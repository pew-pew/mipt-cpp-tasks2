cmake_minimum_required(VERSION 3.0.0)

project(Tuple)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -pedantic --std=c++14")
add_executable(main main.cpp main.cpp tuple.h)

add_executable(tests tests.cpp tuple.h)
target_link_libraries(tests gtest)
