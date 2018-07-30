cmake_minimum_required(VERSION 3.10)
project(crow VERSION 0.0.1)

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
cmake_host_system_information(RESULT NLOHMANN_CROW_OS_VERSION QUERY OS_VERSION)

# uname
find_program(UNAME_TOOL uname)
execute_process(COMMAND ${UNAME_TOOL} -a
        OUTPUT_VARIABLE NLOHMANN_CROW_UNAME OUTPUT_STRIP_TRAILING_WHITESPACE)

# compiler
if(MSVC)
    execute_process(COMMAND ${CMAKE_CXX_COMPILER}
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

if(WIN32)
    include(ExternalProject)
    ExternalProject_Add(
            curl_external_project
            URL http://curl.haxx.se/download/curl-7.61.0.tar.gz
            URL_MD5 ef343f64daab4691f528697b58a2d984
            PREFIX curl
            CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/curl -DBUILD_CURL_EXE=OFF -DBUILD_TESTING=OFF -DCURL_STATICLIB=ON -DCURL_CA_PATH_SET=OFF -DCMAKE_USE_OPENSSL=ON -DHTTP_ONLY=ON
    )

    set(CURL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/curl/include)
    add_library(wincurl)
    add_dependencies(wincurl curl_external_project)
    target_link_libraries(wincurl ${CMAKE_CURRENT_BINARY_DIR}/curl/lib/libcurl.a)
endif()

find_package(CURL)
find_package(Threads)


#################
# documentation #
#################

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(doc
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else()
    message("Doxygen need to be installed to generate the documentation")
endif()


###########
# testing #
###########

include(CTest)
if(BUILD_TESTING)
    enable_testing()

    add_executable(tests tests/unittests.cpp)
    set_target_properties(tests PROPERTIES CXX_STANDARD 11)
    target_include_directories(tests PUBLIC tests include ${CMAKE_CURRENT_BINARY_DIR}/include)
    target_link_libraries(tests ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

    add_executable(livetest tests/livetest.cpp)
    set_target_properties(livetest PROPERTIES CXX_STANDARD 11)
    target_include_directories(livetest PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/include ${CURL_INCLUDE_DIR})
    target_link_libraries(livetest ${CURL_LIBRARIES} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${LIBSSH2_LIBRARY} ${OPENSSL_LIBRARIES})
    if(WIN32)
        target_link_libraries(livetest wincurl)
    endif()

    add_test(tests tests)
    add_test(livetest livetest)
endif()
