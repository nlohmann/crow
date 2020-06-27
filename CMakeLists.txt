cmake_minimum_required(VERSION 3.8)
cmake_policy(SET CMP0048 NEW)
project(crow VERSION 2.4.0 LANGUAGES C CXX)

#################
# CMake options #
#################

option(CROW_BUILD_TESTING "build tests" OFF)
option(CROW_BUILD_LOG4CPLUS "build log4cplus example" OFF)
option(CROW_CHECK_COVERAGE "instrument compiler to collect coverage information" OFF)
option(CROW_USE_PACKAGED_CURL_WRAPPER "whether to use the packaged version of curl_wrapper" ON)
option(CROW_USE_PACKAGED_JSON "whether to use the packaged version of nlohmann/json" ON)

################################
# check for required functions #
################################

include(CheckCXXSymbolExists)
check_cxx_symbol_exists(abi::__cxa_demangle "cxxabi.h" NLOHMANN_CROW_HAVE_CXA_DEMANGLE)
check_cxx_symbol_exists(backtrace "execinfo.h" NLOHMANN_CROW_HAVE_BACKTRACE)
check_cxx_symbol_exists(pthread_getname_np "pthread.h" NLOHMANN_CROW_HAVE_PHTREAD_GETNAME_NP)
check_cxx_symbol_exists(pthread_threadid_np "pthread.h" NLOHMANN_CROW_HAVE_PHTREAD_THREADID_NP)

check_cxx_symbol_exists(gmtime_r "time.h" NLOHMANN_CROW_HAVE_GMTIME_R)
if (NOT NLOHMANN_CROW_HAVE_GMTIME_R)
    message(WARNING "Did not find a thread-safe version of gmtime.")
endif()

set(CMAKE_REQUIRED_LIBRARIES dl)
check_cxx_symbol_exists(dladdr "dlfcn.h" NLOHMANN_CROW_HAVE_DLADDR)
unset(CMAKE_REQUIRED_LIBRARIES)

include(CheckSymbolExists)
check_symbol_exists(GetCurrentProcessId "processthreadsapi.h" NLOHMANN_CROW_HAVE_GETCURRENTPROCESSID)
check_symbol_exists(getpid "unistd.h" NLOHMANN_CROW_HAVE_GETPID)

include(CheckIncludeFileCXX)
check_include_file_cxx(unistd.h NLOHMANN_CROW_HAVE_UNISTD_H)

##################################
# collect additional information #
##################################

# CMake
cmake_host_system_information(RESULT NLOHMANN_CROW_HOSTNAME QUERY HOSTNAME)
cmake_host_system_information(RESULT NLOHMANN_CROW_TOTAL_PHYSICAL_MEMORY QUERY TOTAL_PHYSICAL_MEMORY)
cmake_host_system_information(RESULT NLOHMANN_CROW_AVAILABLE_PHYSICAL_MEMORY QUERY AVAILABLE_PHYSICAL_MEMORY)

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

# whoami
find_program(WHOAMI_TOOL whoami)
execute_process(COMMAND ${WHOAMI_TOOL}
        OUTPUT_VARIABLE NLOHMANN_CROW_WHOAMI OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE "\\" "\\\\" NLOHMANN_CROW_WHOAMI "${NLOHMANN_CROW_WHOAMI}")

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

# curl
find_program(CURL_TOOL curl)
execute_process(COMMAND ${CURL_TOOL} --max-time 1 ifconfig.me/ip
        OUTPUT_VARIABLE NLOHMANN_CROW_IP_ADDRESS ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# /sys/class/dmi
execute_process(COMMAND cat /sys/class/dmi/id/product_name OUTPUT_VARIABLE NLOHMANN_CROW_SYS_PRODUCT_NAME ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND cat /sys/class/dmi/id/product_family OUTPUT_VARIABLE NLOHMANN_CROW_SYS_PRODUCT_FAMILY ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND cat /sys/class/dmi/id/sys_vendor OUTPUT_VARIABLE NLOHMANN_CROW_SYS_VENDOR ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

################
# dependencies #
################

find_package(Threads)

if (CROW_USE_PACKAGED_JSON)
    set(JSON_BuildTests OFF CACHE INTERNAL "")
    message(STATUS "Using packaged nlohmann/json")
    add_subdirectory(external/json)
endif()

if (CROW_USE_PACKAGED_CURL_WRAPPER)
    set(CURL_WRAPPER_USE_PACKAGED_JSON OFF CACHE INTERNAL "")
    message(STATUS "Using packaged curl_wrapper")
    add_subdirectory(external/curl_wrapper)
endif()

#include(tools/cmake/coverage.cmake)

########################
# create config header #
########################

add_subdirectory(external/backward)

if (${LIBBFD_LIBRARY})
    set(NLOHMANN_CROW_HAVE_LIBBFD)
endif()
if (LIBDW_LIBRARY)
    set(NLOHMANN_CROW_HAVE_LIBDW)
endif()

configure_file(src/crow_config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/include/crow/crow_config.hpp
)

###########
# library #
###########

add_library(crow
    src/client/client.cpp
    src/crow.cpp
    src/hub/hub.cpp
    src/hub/scope.cpp
    src/types/event.cpp
    src/types/helpers/base_types.cpp
    src/types/helpers/helpers.cpp
    src/types/interfaces/breadcrumbs.cpp
    src/types/interfaces/context.cpp
    src/types/interfaces/exception.cpp
    src/types/interfaces/gpu.cpp
    src/types/interfaces/http.cpp
    src/types/interfaces/sdk.cpp
    src/types/interfaces/stacktrace.cpp
    src/types/interfaces/template.cpp
    src/types/interfaces/thread.cpp
    src/types/interfaces/user.cpp
    src/utils/utils.cpp
)
target_compile_features(crow PUBLIC cxx_std_11)
target_include_directories(crow PUBLIC include PRIVATE external ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_libraries(crow PUBLIC curl_wrapper ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES} nlohmann_json::nlohmann_json backward)

if (LIBBFD_LIBRARY)
    target_link_libraries(crow PUBLIC ${LIBBFD_LIBRARY})
endif()
if (LIBDW_LIBRARY)
    target_link_libraries(crow PUBLIC ${LIBDW_LIBRARY})
endif()

#################
# documentation #
#################

add_subdirectory(docs)

###########
# testing #
###########

if (CROW_BUILD_LOG4CPLUS)
    set(UNICODE OFF CACHE INTERNAL "")
    set(LOG4CPLUS_BUILD_TESTING OFF CACHE INTERNAL "")
    set(WITH_UNIT_TESTS OFF CACHE INTERNAL "")
    set(LOG4CPLUS_BUILD_LOGGINGSERVER OFF CACHE INTERNAL "")
    set(LOG4CPLUS_ENABLE_DECORATED_LIBRARY_NAME OFF CACHE INTERNAL "")
    add_subdirectory(external/log4cplus log4cplus EXCLUDE_FROM_ALL)
endif()

add_subdirectory(external/doctest)
add_subdirectory(tests)
