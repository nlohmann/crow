cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0048 NEW)
project(crow VERSION 0.0.6 LANGUAGES CXX)

#################
# CMake options #
#################

option(CROW_EXTERNAL_ZLIB_PROJECT "the path to zlib" OFF)
option(CROW_EXTERNAL_CURL_PROJECT "the path to curl" OFF)
option(CROW_BUILD_TESTING "build tests" OFF)
option(CROW_BUILD_LOG4CPLUS "build log4cplus example" OFF)
option(CROW_CHECK_COVERAGE "instrument compiler to collect coverage information" OFF)

################################
# check for required functions #
################################

include(CheckSymbolExists)
check_symbol_exists(abi::__cxa_demangle "cxxabi.h" NLOHMANN_CROW_HAVE_CXA_DEMANGLE)
check_symbol_exists(backtrace "execinfo.h" NLOHMANN_CROW_HAVE_BACKTRACE)
check_symbol_exists(dladdr "dlfcn.h" NLOHMANN_CROW_HAVE_DLADDR)

##################################
# collect additional information #
##################################

# CMake
cmake_host_system_information(RESULT NLOHMANN_CROW_HOSTNAME QUERY HOSTNAME)
cmake_host_system_information(RESULT NLOHMANN_CROW_TOTAL_PHYSICAL_MEMORY QUERY TOTAL_PHYSICAL_MEMORY)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.10.0")
    cmake_host_system_information(RESULT NLOHMANN_CROW_OS_RELEASE QUERY OS_RELEASE)
    string(STRIP "${NLOHMANN_CROW_OS_RELEASE}" NLOHMANN_CROW_OS_RELEASE)
    cmake_host_system_information(RESULT NLOHMANN_CROW_OS_VERSION QUERY OS_VERSION)
    string(STRIP "${NLOHMANN_CROW_OS_VERSION}" NLOHMANN_CROW_OS_VERSION)
endif()

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

# check for MinGW which has a broken <random> header
if(MINGW)
    set(NLOHMANN_CROW_MINGW ${MINGW})
endif()

########################
# create config header #
########################

configure_file(src/crow_config.hpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/crow_config.hpp)

################
# dependencies #
################

if(CROW_EXTERNAL_ZLIB_PROJECT)
    add_subdirectory(${CROW_EXTERNAL_ZLIB_PROJECT})
    set(ZLIB_INCLUDE_DIRS ${CROW_EXTERNAL_ZLIB_PROJECT})
    set(ZLIB_LIBRARIES zlibstatic)
else()
    find_package(ZLIB REQUIRED ${CROW_EXTERNAL_ZLIB_PROJECT})
endif()

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

add_library(crow src/crow.cpp src/crow_utilities.cpp src/crow_utilities.hpp)
target_compile_features(crow PUBLIC cxx_std_11)
target_include_directories(crow PUBLIC include external PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CURL_INCLUDE_DIR} ${ZLIB_INCLUDE_DIRS})
target_link_libraries(crow ${CURL_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})

#################
# documentation #
#################

add_subdirectory(doc)

###########
# testing #
###########

add_subdirectory(tests)
