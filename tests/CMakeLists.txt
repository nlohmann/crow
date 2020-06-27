include(CTest)
if(CROW_BUILD_TESTING AND (BUILD_TESTING OR CROW_EXTERNAL_CURL_PROJECT))
    enable_testing()

    # see https://stackoverflow.com/a/33694733
    add_executable(test_wrapper src/test_wrapper.c)
    macro(add_test_crashed name command)
        add_test(NAME ${name} COMMAND test_wrapper ${command} ${ARGN})
        set_tests_properties(${name} PROPERTIES WILL_FAIL TRUE)
    endmacro(add_test_crashed)

    add_executable(crow_unittests src/unit_tests.cpp)
    target_link_libraries(crow_unittests doctest crow)
    add_test(NAME crow_unittests COMMAND crow_unittests)

    add_executable(crow_integration_tests src/integration_tests.cpp)
    target_link_libraries(crow_integration_tests doctest crow)
    add_test(NAME crow_integration_tests COMMAND crow_integration_tests)

    add_executable(crow_uncaught_exception src/uncaught_exception.cpp)
    target_link_libraries(crow_uncaught_exception crow)
    add_test_crashed(crow_uncaught_exception crow_uncaught_exception)

    add_executable(crow_signal src/signal.cpp)
    target_link_libraries(crow_signal crow)
    add_test_crashed(crow_signal crow_signal)

    add_executable(crow_signal_sigsegv src/signal_sigsegv.cpp)
    target_link_libraries(crow_signal_sigsegv crow)
    add_test_crashed(crow_signal_sigsegv crow_signal_sigsegv)

    add_executable(crow_errno src/errno.cpp)
    target_link_libraries(crow_errno crow)
    add_test(NAME crow_errno COMMAND crow_errno)

    if (CROW_BUILD_LOG4CPLUS)
        add_executable(crow_log4cplus_example src/log4cplus_example.cpp)
        target_link_libraries(crow_log4cplus_example crow log4cplus)
        add_test(NAME crow_log4cplus_example COMMAND crow_log4cplus_example)
    endif()
endif()
