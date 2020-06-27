#################
# code coverage #
#################

option(CURL_WRAPPER_COVERAGE "instrument code to create coverage information" OFF)
if (CURL_WRAPPER_COVERAGE)
    message(STATUS "🔧 Building test suite with coverage information; now build, run the unit tests and call `make curl_wrapper_coverage_report`")

    # add compiler flags to collect coverage information
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0 -fprofile-arcs -ftest-coverage -Wno-unused-command-line-argument")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -g -O0 -fprofile-arcs -ftest-coverage -Wno-unused-command-line-argument")

    # collect all source files that we are interested in
    file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/include/http/* ${CMAKE_SOURCE_DIR}/src/*)

    # define target to run lcov to collect coverage information, filter unwanted files, and create HTML report
    add_custom_target(curl_wrapper_coverage_report
            COMMAND lcov --directory . --capture --output-file coverage.info --rc lcov_branch_coverage=1
            COMMAND lcov -e coverage.info ${SOURCE_FILES} --output-file coverage.info.filtered --rc lcov_branch_coverage=1
            COMMAND ${CMAKE_SOURCE_DIR}/tools/imapdl/filterbr.py coverage.info.filtered > coverage.info.filtered.noexcept
            COMMAND ${CMAKE_SOURCE_DIR}/tools/lcov-to-cobertura-xml/lcov_cobertura.py --base-dir ${CMAKE_SOURCE_DIR} --demangle coverage.info.filtered.noexcept
            COMMAND genhtml --title "${CMAKE_PROJECT_NAME}" --legend --demangle-cpp --output-directory coverage_report --show-details --branch-coverage coverage.info.filtered.noexcept
            COMMENT "Generating HTML report coverage_report/index.html"
            )
endif()
