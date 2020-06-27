if (CURL_WRAPPER_TESTS)
    enable_testing()

    # unit tests
    add_executable(curl_wrapper_unit_tests unit_tests.cpp)
    target_include_directories(curl_wrapper_unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/external)
    target_link_libraries(curl_wrapper_unit_tests curl_wrapper)

    # example from the README
    add_executable(curl_wrapper_example ${CMAKE_SOURCE_DIR}/docs/assets/example.cpp)
    target_link_libraries(curl_wrapper_example curl_wrapper)

    add_test(NAME curl_wrapper_unit_tests COMMAND curl_wrapper_unit_tests)
    add_test(NAME curl_wrapper_example COMMAND curl_wrapper_example)
endif()
