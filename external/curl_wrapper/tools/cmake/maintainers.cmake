#########
# tools #
#########

find_program(DOXYGEN_TOOL doxygen)
execute_process(COMMAND ${DOXYGEN_TOOL} --version OUTPUT_VARIABLE DOXYGEN_VERSION ERROR_VARIABLE DOXYGEN_VERSION)

find_program(CLANG_FORMAT_TOOL clang-format)
execute_process(COMMAND ${CLANG_FORMAT_TOOL} --version OUTPUT_VARIABLE CLANG_FORMAT_VERSION ERROR_VARIABLE CLANG_FORMAT_VERSION)

find_program(CLANG_TIDY_TOOL clang-tidy)
execute_process(COMMAND ${CLANG_TIDY_TOOL} --version OUTPUT_VARIABLE CLANG_TIDY_VERSION ERROR_VARIABLE CLANG_TIDY_VERSION)

find_program(CPPCHECK_TOOL cppcheck)
execute_process(COMMAND ${CPPCHECK_TOOL} --version OUTPUT_VARIABLE CPPCHECK_VERSION ERROR_VARIABLE CPPCHECK_VERSION)

find_program(OCLINT_TOOL oclint-json-compilation-database)
execute_process(COMMAND ${OCLINT_TOOL} -- --version OUTPUT_VARIABLE OCLINT_VERSION ERROR_VARIABLE OCLINT_VERSION)

find_program(CPPLINT_TOOL cpplint.py PATH ${PROJECT_SOURCE_DIR}/tools/cpplint)

#############
# utilities #
#############

# find GNU sed tool on Linux and macOS
find_program(GNUSED_TOOL NAMES gsed gnused sed)

# find a way to create a Python virtual environment
find_program(VIRTUALENV_TOOL virtualenv)
if (${VIRTUALENV_TOOL} STREQUAL "VIRTUALENV_TOOL-NOTFOUND")
    set(VIRTUALENV_TOOL python3 -mvenv)
else()
    set(VIRTUALENV_TOOL ${VIRTUALENV_TOOL} -p python3)
endif()

################
# Clang-format #
################

if (NOT ${CLANG_FORMAT_TOOL} STREQUAL "CLANG_FORMAT_TOOL-NOTFOUND")
    # which files should be indented
    file(GLOB_RECURSE ALL_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/include/*.hpp
        ${PROJECT_SOURCE_DIR}/src/*.cpp
        ${PROJECT_SOURCE_DIR}/tests/*.cpp
    )

    # apply Clang-format
    add_custom_target(curl_wrapper_format
        COMMENT "indenting source files"
        COMMAND ${CLANG_FORMAT_TOOL} -i -verbose ${ALL_SOURCE_FILES}
    )

    # apply Clang-format and fail in case there were changes
    add_custom_target(curl_wrapper_format_check
        COMMENT "checking source file indentation"
        # check if there are uncomitted changes
        COMMAND git diff > baseline.txt
        # indent source files with Clang-Format
        COMMAND ${CLANG_FORMAT_TOOL} -i ${ALL_SOURCE_FILES}
        # check if indentation made a difference
        COMMAND git diff > indentation_changes.txt
        # compare the git diff outputs
        COMMAND diff baseline.txt indentation_changes.txt > changes.txt || true
        # display the differences (empty if there are no changes)
        COMMAND cat changes.txt
        # let target fail if report file is not empty (i.e., indentation yielded changes)
        COMMAND test ! -s changes.txt
        # clean up
        COMMAND rm -f changes.txt baseline.txt indentation_changes.txt
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif ()

############
# Cppcheck #
############

# execute cppcheck
if (NOT ${CPPCHECK_TOOL} STREQUAL "CPPCHECK_TOOL-NOTFOUND")
    add_custom_target(cppcheck_defines.txt
        COMMENT "collecting default defines from ${CMAKE_CXX_COMPILER}"
        COMMAND ${CMAKE_CXX_COMPILER} -dM -E -x c++ -std=c++14 - < /dev/null | ${GNUSED_TOOL} -e "s/\#define /-D/;s/ .*//" | xargs > ${CMAKE_BINARY_DIR}/cppcheck_defines.txt
        VERBATIM
    )

    add_custom_target(cppcheck.log
        COMMENT "analyzing sources with Cppcheck"
        # read defines from file to used them in Cppcheck call
        DEPENDS cppcheck_defines.txt
        COMMAND ${CPPCHECK_TOOL} `cat cppcheck_defines.txt` --relative-paths=${CMAKE_SOURCE_DIR} --inline-suppr --enable=all --inconclusive --suppress=missingIncludeSystem:* --std=c++14 --template=gcc --output-file=cppcheck.log --rule-file=${CMAKE_SOURCE_DIR}/tools/cppcheck/cppcheck_rules.xml --quiet ${PROJECT_SOURCE_DIR}/include/http/* ${PROJECT_SOURCE_DIR}/src/*
        # display output of Cppcheck (empty if no issue was found)
        COMMAND cat cppcheck.log
        # let target fail if report file is not empty (i.e., issues were found)
        COMMAND test ! -s cppcheck.log
    )
endif()

##############
# Clang-Tidy #
##############

if (NOT ${CLANG_TIDY_TOOL} STREQUAL "CLANG_TIDY_TOOL-NOTFOUND")
    file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/include/http/*.hpp ${CMAKE_SOURCE_DIR}/src/*.cpp)

    foreach(SOURCE_FILE ${SOURCE_FILES})
        # create an MD5 hash from the source file name to have unique report names
        string(MD5 SOURCE_FILE_HASH ${SOURCE_FILE})
        # get source filename without directory as prefix for the report name
        get_filename_component(SOURCE_FILE_NAME ${SOURCE_FILE} NAME)
        # get relative path to the source file for better status messages
        file(RELATIVE_PATH SOURCE_FILE_RELATIVE ${CMAKE_SOURCE_DIR} ${SOURCE_FILE})
        # put all Clang-Tidy output in a dedicated directory
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/clang-tidy)
        # the Clang-Tidy report file name
        set(CLANG_TIDY_OUTPUT_FILE clang-tidy/${SOURCE_FILE_NAME}_${SOURCE_FILE_HASH}.log)

        add_custom_command(
            COMMENT "analyzing ${SOURCE_FILE_RELATIVE} with Clang-Tidy"
            # create a report file per source file
            OUTPUT ${CLANG_TIDY_OUTPUT_FILE}
            # re-generate report if source file changes
            DEPENDS ${SOURCE_FILE}
            COMMAND touch ${CLANG_TIDY_OUTPUT_FILE}
            # run Clang-Tidy on source file and store result in result file
            COMMAND ${CLANG_TIDY_TOOL} -p=${PROJECT_BINARY_DIR} ${SOURCE_FILE} -header-filter=${CMAKE_SOURCE_DIR}/src/bumblebee/[^i].* -quiet 2> /dev/null > ${CLANG_TIDY_OUTPUT_FILE} || ${CLANG_TIDY_TOOL} -p=${PROJECT_BINARY_DIR} ${SOURCE_FILE} -header-filter=${CMAKE_SOURCE_DIR}/src/bumblebee/[^i].*
            # display output of Clang-Tidy (empty if no issue was found)
            COMMAND cat ${CLANG_TIDY_OUTPUT_FILE}
            # making directories relative to source folder to make report independent of current machine
            COMMAND ${GNUSED_TOOL} -i s'|${PROJECT_SOURCE_DIR}/||g' ${CLANG_TIDY_OUTPUT_FILE}
        )

        # the list of all Clang-Tidy report files
        list(APPEND CLANG_TIDY_OUTPUT_FILES ${CLANG_TIDY_OUTPUT_FILE})
    endforeach()

    add_custom_target(clang-tidy.log
        COMMENT "creating Clang-Tidy report"
        # build reports for all soure files
        DEPENDS ${CLANG_TIDY_OUTPUT_FILES}
        # build a single report from all individual report files
        COMMAND cat ${CLANG_TIDY_OUTPUT_FILES} > clang-tidy.log
        # let target fail if report file is not empty (i.e., issues were found)
        COMMAND test ! -s clang-tidy.log
    )
endif()

###########
# cpplint #
###########

if (NOT ${CPPLINT_TOOL} STREQUAL "CPPLINT_TOOL-NOTFOUND")
    add_custom_target(cpplint.log
        COMMENT "analyzing sources with cpplint"
        COMMAND ${CPPLINT_TOOL} --filter=-whitespace,-legal,-build/include_order,-readability/alt_tokens,-build/c++11,-runtime/references,-readability/nolint --quiet --recursive ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src 2>&1 | sort | tee cpplint.log
        # let target fail if report file is not empty (i.e., issues were found)
        COMMAND test ! -s cpplint.log
    )
endif()

##########
# OCLint #
##########

if (NOT ${OCLINT_TOOL} STREQUAL "OCLINT_TOOL-NOTFOUND")
    add_custom_target(oclint.log
        COMMENT "analyzing sources with OCLint"
        COMMAND ${OCLINT_TOOL} -include ${CMAKE_SOURCE_DIR}/include/http ${CMAKE_SOURCE_DIR}/src/* -- -report-type text -enable-global-analysis -o oclint.log -max-priority-1=10000 -max-priority-2=10000 -max-priority-3=10000 -disable-rule TooFewBranchesInSwitchStatement -disable-rule ShortVariableName -disable-rule LongLine -disable-rule LongVariableName -disable-rule DoubleNegative -disable-rule MultipleUnaryOperator -disable-rule LongMethod -disable-rule HighNcssMethod
        COMMAND cat oclint.log
    )
endif()

#############
# SonarQube #
#############

# collect system includes (snippet from https://trac.webkit.org/changeset/205672/webkit/trunk/Source/cmake/OptionsCommon.cmake)
macro(DETERMINE_GCC_SYSTEM_INCLUDE_DIRS _lang _compiler _flags _result)
    file(WRITE "${CMAKE_BINARY_DIR}/CMakeFiles/dummy" "\n")
    separate_arguments(_buildFlags UNIX_COMMAND "${_flags}")
    execute_process(COMMAND ${_compiler} ${_buildFlags} -v -E -x ${_lang} -dD dummy
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles OUTPUT_QUIET
                    ERROR_VARIABLE _gccOutput)
    file(REMOVE "${CMAKE_BINARY_DIR}/CMakeFiles/dummy")
    if ("${_gccOutput}" MATCHES "> search starts here[^\n]+\n *(.+) *\n *End of (search) list")
        set(${_result} ${CMAKE_MATCH_1})
        string(REPLACE "\n" " " ${_result} "${${_result}}")
        string(REPLACE "(framework directory)" " " ${_result} "${${_result}}")
        separate_arguments(${_result})
    endif ()
endmacro()

# collect all include directories; start with system includes
DETERMINE_GCC_SYSTEM_INCLUDE_DIRS("c++" "${CMAKE_CXX_COMPILER}" "${CMAKE_CXX_FLAGS}" SYSTEM_INCLUDE_DIRS)
set(INCLUDE_DIRECTORIES ${SYSTEM_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

# add thirdparty and other include directories
list(APPEND INCLUDE_DIRECTORIES
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/external/doctest
    ${CMAKE_SOURCE_DIR}/external/json
)

##############
# quickcheck #
##############

# a target for the quickcheck stage in the CI
add_custom_target(curl_wrapper_quickcheck
    COMMENT "running the quickcheck"
    DEPENDS curl_wrapper_documentation
    DEPENDS cppcheck.log
    DEPENDS cpplint.log
    DEPENDS clang-tidy.log
)
