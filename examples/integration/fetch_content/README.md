# Integration using FetchContent

The integration using the [`FetchContent`][1] module from CMake is very easy but requires CMake 3.14 or above. If you are stuck with previous versions of CMake, you may use the example from [../external_project].


[1]: https://cmake.org/cmake/help/v3.18/module/FetchContent.html

```cmake
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
```