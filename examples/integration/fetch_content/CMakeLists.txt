cmake_minimum_required(VERSION 3.14)

project(crow_example)

include(FetchContent)
FetchContent_Declare(crow
  GIT_REPOSITORY    https://github.com/nlohmann/crow
  GIT_TAG           v0.0.6
)
FetchContent_MakeAvailable(crow)

add_executable(example example.cpp)
target_link_libraries(example crow)