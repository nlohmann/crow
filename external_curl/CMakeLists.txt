cmake_minimum_required(VERSION 3.10)

include(ExternalProject)
ExternalProject_Add(
        curl_external_project
        URL http://curl.haxx.se/download/curl-7.61.0.tar.gz
        URL_MD5 ef343f64daab4691f528697b58a2d984
        PREFIX curl
        CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/curl -DBUILD_CURL_EXE=OFF -DBUILD_TESTING=OFF -DCURL_STATICLIB=ON -DCURL_CA_PATH_SET=OFF -DCMAKE_USE_OPENSSL=OFF -DHTTP_ONLY=ON
)

add_custom_target(curl ALL DEPENDS curl_external_project)
