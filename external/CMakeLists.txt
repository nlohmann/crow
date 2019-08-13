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
