# Integration as external project

The integration as external project is a bit more complicated.

*Note*: If you are using CMake version 3.14 or above, refer to the [fetch_content example][../fetch_content].

Example:

```cmake
cmake_minimum_required(VERSION 3.10)

project(crow_example)

# required thirdparty libraries
find_package(CURL)
find_package(ZLIB REQUIRED)
find_package(Threads)

# download Crow from the GitHub repository
include(ExternalProject)
ExternalProject_Add(crow_project
        GIT_REPOSITORY https://github.com/nlohmann/crow
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/crow
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/crow -DCROW_BUILD_TESTING=OFF
        BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/crow/lib/${CMAKE_STATIC_LIBRARY_PREFIX}crow${CMAKE_STATIC_LIBRARY_SUFFIX}
        )
ExternalProject_Get_Property(crow_project install_dir)
add_library(crow STATIC IMPORTED)
set_target_properties(crow PROPERTIES IMPORTED_LOCATION ${install_dir}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}crow${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(crow crow_project)
include_directories(${install_dir}/include)

# link agains Crow, and also its required thirdparty libraries
add_executable(example example.cpp)
target_link_libraries(example crow ${CURL_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})
set_target_properties(example PROPERTIES CXX_STANDARD 11)
```
