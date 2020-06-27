cmake_minimum_required(VERSION 3.0)
project(curl_wrapper VERSION 0.3.0 LANGUAGES CXX)

#################
# CMake options #
#################

option(CURL_WRAPPER_TESTS "compile tests" OFF)
option(CURL_WRAPPER_USE_PACKAGED_JSON "whether to use the packaged version of nlohmann/json" ON)

################
# dependencies #
################

find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)

if (CURL_WRAPPER_USE_PACKAGED_JSON)
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    message(STATUS "Using packaged nlohmann/json")
    add_subdirectory(external/json)
endif()

###########
# library #
###########

add_library(curl_wrapper src/curl_initializer.cpp src/exception.cpp src/gzip_wrapper.cpp src/request.cpp src/response.cpp src/status.cpp)
target_include_directories(curl_wrapper PUBLIC include)
target_compile_features(curl_wrapper PUBLIC cxx_std_11)
target_link_libraries(curl_wrapper PUBLIC ${CURL_LIBRARIES} ${ZLIB_LIBRARIES} nlohmann_json::nlohmann_json)

#################
# documentation #
#################

add_subdirectory(docs)

###########
# testing #
###########

# create a compilation database (needed for instance by clang-tidy)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(tools/cmake/coverage.cmake)

add_subdirectory(tests)

get_directory_property(project_was_included_by_other_project PARENT_DIRECTORY)
if(NOT project_was_included_by_other_project)
    include(tools/cmake/maintainers.cmake)
endif()
