#################
# documentation #
#################

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(crow_documentation
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/doc
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
endif()
