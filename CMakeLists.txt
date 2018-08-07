cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0048 NEW)
project(crow VERSION 0.0.4)

##############################
# check for required headers #
##############################

include(CheckIncludeFiles)
check_include_files(cxxabi.h NLOHMANN_CROW_HAVE_CXXABI_H)
check_include_files(execinfo.h NLOHMANN_CROW_HAVE_EXECINFO_H)
check_include_files(dlfcn.h NLOHMANN_CROW_HAVE_DLFCN_H)

##################################
# collect additional information #
##################################

# CMake
cmake_host_system_information(RESULT NLOHMANN_CROW_HOSTNAME QUERY HOSTNAME)
cmake_host_system_information(RESULT NLOHMANN_CROW_TOTAL_PHYSICAL_MEMORY QUERY TOTAL_PHYSICAL_MEMORY)
cmake_host_system_information(RESULT NLOHMANN_CROW_OS_RELEASE QUERY OS_RELEASE)
string(STRIP "${NLOHMANN_CROW_OS_RELEASE}" NLOHMANN_CROW_OS_RELEASE)
cmake_host_system_information(RESULT NLOHMANN_CROW_OS_VERSION QUERY OS_VERSION)
string(STRIP "${NLOHMANN_CROW_OS_VERSION}" NLOHMANN_CROW_OS_VERSION)

# uname
find_program(UNAME_TOOL uname)
execute_process(COMMAND ${UNAME_TOOL} -a
        OUTPUT_VARIABLE NLOHMANN_CROW_UNAME OUTPUT_STRIP_TRAILING_WHITESPACE)

# systeminfo
find_program(SYSTEMINFO_TOOL systeminfo)
execute_process(COMMAND ${SYSTEMINFO_TOOL}
        OUTPUT_VARIABLE NLOHMANN_CROW_SYSTEMINFO OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NLOHMANN_CROW_SYSTEMINFO)
    string(REGEX MATCHALL "OS [^\n]*" NLOHMANN_CROW_SYSTEMINFO ${NLOHMANN_CROW_SYSTEMINFO})
    string(REPLACE ";" "\\n" NLOHMANN_CROW_SYSTEMINFO "${NLOHMANN_CROW_SYSTEMINFO}")
endif()

# compiler version output
if(MSVC)
    execute_process(COMMAND msbuild /version
        OUTPUT_VARIABLE NLOHMANN_CROW_CXX OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
            OUTPUT_VARIABLE NLOHMANN_CROW_CXX OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()
string(REGEX REPLACE "\n" "\\\\n" NLOHMANN_CROW_CXX ${NLOHMANN_CROW_CXX})

# sysctl
find_program(SYSCTL_TOOL sysctl)
execute_process(COMMAND ${SYSCTL_TOOL} hw.model
        OUTPUT_VARIABLE NLOHMANN_CROW_SYSCTL_HW_MODEL ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NLOHMANN_CROW_SYSCTL_HW_MODEL)
    string(REGEX REPLACE ".*: (.*)" "\\1" NLOHMANN_CROW_SYSCTL_HW_MODEL ${NLOHMANN_CROW_SYSCTL_HW_MODEL})
endif()

########################
# create config header #
########################

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/crow/config.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/include/crow/config.h)

################
# dependencies #
################

option(CROW_EXTERNAL_ZLIB_PROJECT "the path to zlib" OFF)
if(CROW_EXTERNAL_ZLIB_PROJECT)
    add_subdirectory(${CROW_EXTERNAL_ZLIB_PROJECT})
    set(ZLIB_INCLUDE_DIRS ${CROW_EXTERNAL_ZLIB_PROJECT})
    set(ZLIB_LIBRARIES zlibstatic)
else()
    find_package(ZLIB REQUIRED ${CROW_EXTERNAL_ZLIB_PROJECT})
endif()

option(CROW_EXTERNAL_CURL_PROJECT "the path to curl" OFF)
if(CROW_EXTERNAL_CURL_PROJECT)
    set(BUILD_TESTING OFF CACHE INTERNAL "")
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    set(CURL_STATICLIB ON CACHE INTERNAL "")
    set(BUILD_CURL_EXE OFF CACHE INTERNAL "")
    set(CURL_CA_PATH_SET OFF CACHE INTERNAL "")
    set(CMAKE_USE_OPENSSL ON CACHE INTERNAL "")
    set(HTTP_ONLY ON CACHE INTERNAL "")
    add_definitions(-DCURL_STATICLIB)
    add_subdirectory(${CROW_EXTERNAL_CURL_PROJECT})
    set(CURL_INCLUDE_DIR ${CROW_EXTERNAL_CURL_PROJECT}/include)
    set(CURL_LIBRARIES libcurl)
else()
    find_package(CURL)
endif()

find_package(Threads)

###########
# library #
###########

add_library(crow src/crow.cpp)
set_target_properties(crow PROPERTIES CXX_STANDARD 11)
target_include_directories(crow PUBLIC include PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include ${CURL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS})
target_link_libraries(crow ${CURL_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})

#################
# documentation #
#################

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(crow_documentation
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
endif()

###########
# testing #
###########

include(CTest)
option(CROW_BUILD_TESTING "build tests" ON)
option(CROW_BUILD_LOG4CPLUS "build log4cplus example" OFF)
if(CROW_BUILD_TESTING AND (BUILD_TESTING OR CROW_EXTERNAL_CURL_PROJECT))
    enable_testing()

    get_target_property(CROW_SOURCES crow SOURCES)
    add_executable(tests tests/unittests.cpp tests/thirdparty/curl_wrapper/curl_wrapper.cpp ${CROW_SOURCES})
    set_target_properties(tests PROPERTIES CXX_STANDARD 11)
    target_include_directories(tests PUBLIC tests include ${CMAKE_CURRENT_BINARY_DIR}/include)
    target_link_libraries(tests ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

    add_executable(livetest tests/livetest.cpp)
    set_target_properties(livetest PROPERTIES CXX_STANDARD 11)
    target_include_directories(livetest PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include)
    target_link_libraries(livetest crow)

    add_test(NAME tests COMMAND tests -s)
    add_test(NAME livetest COMMAND livetest)

    if(CROW_BUILD_LOG4CPLUS)
        include(ExternalProject)
        ExternalProject_Add(log4cplus_project
                URL https://github.com/log4cplus/log4cplus/releases/download/REL_2_0_1/log4cplus-2.0.1.zip
                INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/log4cplus
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/log4cplus -DBUILD_SHARED_LIBS=OFF -DLOG4CPLUS_ENABLE_DECORATED_LIBRARY_NAME=OFF -DUNICODE=OFF -DWITH_UNIT_TESTS=OFF
                BUILD_BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/log4cplus/lib/${CMAKE_STATIC_LIBRARY_PREFIX}log4cplus${CMAKE_STATIC_LIBRARY_SUFFIX}
                )
        ExternalProject_Get_Property(log4cplus_project install_dir)
        add_library(log4cplus STATIC IMPORTED)
        set_target_properties(log4cplus PROPERTIES IMPORTED_LOCATION ${install_dir}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}log4cplus${CMAKE_STATIC_LIBRARY_SUFFIX})
        add_dependencies(log4cplus log4cplus_project)

        add_executable(log4cplus_example examples/log4cplus/example.cpp)
        set_target_properties(log4cplus_example PROPERTIES CXX_STANDARD 11)
        target_include_directories(log4cplus_example PUBLIC include ${install_dir}/include)
        target_link_libraries(log4cplus_example log4cplus crow)

        add_test(NAME log4cplus_example COMMAND log4cplus_example)
    endif()
endif()

############
# coverage #
############

option(CROW_CHECK_COVERAGE "instrument compiler to collect coverage information" OFF)
if(CROW_CHECK_COVERAGE)
    message(STATUS "Building test suite with coverage information; now build, run the unit tests and call `make crow_coverage_report`")

    # add compiler flags to collect coverage information
    set(CMAKE_CXX_FLAGS "--coverage -g -O0 -fprofile-arcs -ftest-coverage -Wno-unused-command-line-argument")

    # collect all source files that we are interested in
    file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/include/crow/* ${CMAKE_SOURCE_DIR}/src/*)

    # define target to run lcov to collect coverage information, filter unwanted files, and create HTML report
    add_custom_target(crow_coverage_report
            COMMAND lcov --directory . --capture --output-file coverage.info --rc lcov_branch_coverage=1
            COMMAND lcov -e coverage.info ${SOURCE_FILES} --output-file coverage.info.filtered --rc lcov_branch_coverage=1
            COMMAND genhtml --title "Crow ${PROJECT_VERSION}" --legend --demangle-cpp --output-directory coverage_report --show-details --branch-coverage coverage.info.filtered
            COMMENT "Generating HTML report coverage_report/index.html")
endif()
