cmake_minimum_required(VERSION 3.10)
project(crow VERSION 0.0.1 LANGUAGES CXX)

# check for headers
include(CheckIncludeFiles)
check_include_files(sys/sysctl.h NLOHMANN_CROW_HAVE_SYSCTL_H)
check_include_files(cxxabi.h NLOHMANN_CROW_HAVE_CXXABI_H)
check_include_files(execinfo.h NLOHMANN_CROW_HAVE_EXECINFO_H)
check_include_files(dlfcn.h NLOHMANN_CROW_HAVE_DLFCN_H)

# query additional information
cmake_host_system_information(RESULT NLOHMANN_CROW_HOSTNAME QUERY HOSTNAME)
cmake_host_system_information(RESULT NLOHMANN_CROW_TOTAL_PHYSICAL_MEMORY QUERY TOTAL_PHYSICAL_MEMORY)

# create config.h header
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/crow/config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/include/crow/config.h)

# collect dependencies
find_package(CURL REQUIRED)
add_subdirectory(include/thirdparty/date)

# documentation
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the documentation")
endif (DOXYGEN_FOUND)

# testing
include(CTest)
if (BUILD_TESTING)
    enable_testing()

    add_executable(tests tests/unittests.cpp)
    set_target_properties(tests PROPERTIES CXX_STANDARD 11)
    target_include_directories(tests PUBLIC tests include ${CMAKE_CURRENT_BINARY_DIR}/include ${CURL_INCLUDE_DIR})
    target_link_libraries(tests ${CURL_LIBRARIES})

    add_test(tests tests)
endif (BUILD_TESTING)
