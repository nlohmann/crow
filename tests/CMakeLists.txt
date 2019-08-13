###########
# testing #
###########

include(CTest)
if(CROW_BUILD_TESTING AND (BUILD_TESTING OR CROW_EXTERNAL_CURL_PROJECT))
    enable_testing()

    add_executable(unittests unittests.cpp)
    target_include_directories(unittests PUBLIC ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
    set_target_properties(unittests PROPERTIES CXX_STANDARD 11)
    target_link_libraries(unittests crow)
    add_test(NAME unittests COMMAND unittests)

    # std::current_exception is broken with MSVC (https://developercommunity.visualstudio.com/content/problem/135332/stdcurrent-exception-returns-null-in-a-stdterminat.html)
    if (NOT MSVC)
        add_executable(uncaught_exception uncaught_exception.cpp)
        set_target_properties(uncaught_exception PROPERTIES CXX_STANDARD 11)
        target_link_libraries(uncaught_exception crow)
        add_test(NAME uncaught_exception COMMAND uncaught_exception)
    endif()

    add_executable(livetest livetest.cpp)
    set_target_properties(livetest PROPERTIES CXX_STANDARD 11)
    target_include_directories(livetest PUBLIC ${CMAKE_BINARY_DIR})
    target_link_libraries(livetest crow)
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

        add_executable(log4cplus_example ${CMAKE_SOURCE_DIR}/examples/log4cplus/example.cpp ${CMAKE_SOURCE_DIR}/include/crow/integrations/loggers.hpp ${CMAKE_SOURCE_DIR}/include/crow/integrations/log4cplus.hpp)
        set_target_properties(log4cplus_example PROPERTIES CXX_STANDARD 11)
        target_include_directories(log4cplus_example PUBLIC include ${install_dir}/include)
        target_link_libraries(log4cplus_example log4cplus crow)
        add_test(NAME log4cplus_example COMMAND log4cplus_example)
    endif()
endif()

############
# coverage #
############

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
