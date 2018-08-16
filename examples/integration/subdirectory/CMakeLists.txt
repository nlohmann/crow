cmake_minimum_required(VERSION 3.10)

project(crow_example)

set(CROW_BUILD_TESTING OFF CACHE INTERNAL "do not compile Crow's test cases")
add_subdirectory(crow)

add_executable(example example.cpp)
target_link_libraries(example crow)
