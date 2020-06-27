find_program(DOXYGEN_TOOL doxygen)

if (NOT ${DOXYGEN_TOOL} STREQUAL "DOXYGEN_TOOL-NOTFOUND")
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(curl_wrapper_documentation
        COMMENT "generating API documentation"
        COMMAND ${DOXYGEN_TOOL} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM
    )
endif()
