FIND_PROGRAM(RST2HTML_EXECUTABLE NAMES rst2html.py rst2html
             DOC "Location of the Python Docutils reStructuredText HTML converter")

# handle the QUIETLY and REQUIRED arguments and set DOCUTILS_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Docutils DEFAULT_MSG RST2HTML_EXECUTABLE)

IF(RST2HTML_EXECUTABLE)
    MACRO(RST2HTML)
        FOREACH(it ${ARGN})
            GET_FILENAME_COMPONENT(basename ${it} NAME_WE)

            SET(infile ${CMAKE_CURRENT_SOURCE_DIR}/${it})
            SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/${basename}.html)

            ADD_CUSTOM_COMMAND(
                OUTPUT ${outfile} 
                COMMAND ${RST2HTML_EXECUTABLE}
                ARGS ${infile} ${outfile}
                DEPENDS ${infile})

            ADD_CUSTOM_TARGET(RstHTMLDocuments_${outfile} ALL DEPENDS ${outfile})
        ENDFOREACH()
    ENDMACRO()
ENDIF()

MARK_AS_ADVANCED(RST2HTML_EXECUTABLE)
