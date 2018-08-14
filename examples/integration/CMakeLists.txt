cmake_minimum_required(VERSION 3.10)

project(crow_example)

include(ExternalProject)
ExternalProject_Add(crow_project
        GIT_REPOSITORY https://github.com/nlohmann/crow
        INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/crow
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/crow -DCROW_BUILD_TESTING=OFF
        BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/log4cplus/lib/${CMAKE_SHARED_LIBRARY_PREFIX}crow${CMAKE_SHARED_LIBRARY_SUFFIX}
        )
ExternalProject_Get_Property(crow_project install_dir)
add_library(crow SHARED IMPORTED)
set_target_properties(crow PROPERTIES IMPORTED_LOCATION ${install_dir}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}crow${CMAKE_SHARED_LIBRARY_SUFFIX})
add_dependencies(crow crow_project)

add_executable(example example.cpp)
target_link_libraries(example crow)
set_target_properties(example PROPERTIES CXX_STANDARD 11)
