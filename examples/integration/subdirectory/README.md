# Integration as subdirectory

- download a [release](https://github.com/nlohmann/crow/releases) and unzip the directory in your project tree, say `crow`
- add `add_subdirectory(crow)` to your `CMakeLists.txt` file
- add `crow` to the libraries you want to link against

Minimal example:

```cmake
cmake_minimum_required(VERSION 3.10)

project(crow_example)

set(CROW_BUILD_TESTING OFF CACHE INTERNAL "do not compile Crow's test cases")
add_subdirectory(crow)

add_executable(example example.cpp)
target_link_libraries(example crow)
```
