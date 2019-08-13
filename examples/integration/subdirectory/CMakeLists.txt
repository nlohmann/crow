cmake_minimum_required(VERSION 3.10)

project(crow_example)

add_subdirectory(crow)

add_executable(example example.cpp)
target_link_libraries(example crow)
