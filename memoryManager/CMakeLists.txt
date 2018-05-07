cmake_minimum_required(VERSION 3.0.0)

project(MemoryManagers)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -pedantic --std=c++14")
add_executable(main main.cpp memory_manager.cpp memory_manager.h allocated_on.h stack_allocator.h)
