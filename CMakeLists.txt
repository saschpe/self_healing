#
# (C) Copyright Sascha Peilicke 2010.
#
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
project(self_healing)
cmake_minimum_required(VERSION 2.6.4)

set(CMAKE_MODULE_PATH "${self_healing_SOURCE_DIR}/cmake/modules")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -ansi -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DBOOST_SELF_HEALING_DEBUG")

find_package(Boost 1.42.0 REQUIRED)
find_package(Doxygen)
find_package(Docutils)

include_directories(${Boost_INCLUDE_DIRS})

option(BUILD_TESTS "Build tests" on)

if(DOXYGEN_FOUND OR DOCUTILS_FOUND)
    # cleanup $build/doc on "make clean"
    make_directory(${CMAKE_CURRENT_BINARY_DIR}/doc)
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_BINARY_DIR}/doc)
    # make sure there is actually a 'doc' target
    get_target_property(DOC_TARGET doc TYPE)
    if(NOT DOC_TARGET)
        add_custom_target(doc)
    endif()
endif()

# add doxygen source code documentation as target
if(DOXYGEN_FOUND)
    # configure the Doxygen file
    set(DOXYFILE_IN "${CMAKE_CURRENT_SOURCE_DIR}/libs/self_healing/doc/reference.dox")
    set(DOXYFILE_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc/reference")
    configure_file(${DOXYFILE_IN} Doxyfile ESCAPE_QUOTES IMMEDIATE @ONLY)
    # add the target
    add_custom_target(doxygen ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                              COMMENT "Generating Doxygen code documentation")
    add_dependencies(doc doxygen) # add doxygen as dependency to doc-target
endif()

# add other documentation as target
if(DOCUTILS_FOUND)
    add_custom_target(docutils ${RST2HTML_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/README.txt ${CMAKE_CURRENT_BINARY_DIR}/doc/README.html
                               COMMENT "Generating README.html")
    add_dependencies(doc docutils) # add docutils as dependency to doc-target
endif()

# several unit-test targets
if(BUILD_TESTS)
    enable_testing()
    include_directories(.) # to allow global #include

    add_executable(test-reference_wrapper libs/self_healing/test/reference_wrapper.cpp)
    add_test(reference_wrapper ${EXECUTABLE_OUTPUT_PATH}/test-reference_wrapper)

    add_executable(test-checksummed_array libs/self_healing/test/checksummed_array.cpp)
    add_test(checksummed_array ${EXECUTABLE_OUTPUT_PATH}/test-checksummed_array)
    add_executable(test-checksummed_array-iterators libs/self_healing/test/checksummed_array-iterators.cpp)
    add_test(checksummed_array-iterators ${EXECUTABLE_OUTPUT_PATH}/test-checksummed_array-iterators)
    add_executable(test-checksummed_array-faults libs/self_healing/test/checksummed_array-faults.cpp)
    add_test(checksummed_array-faults ${EXECUTABLE_OUTPUT_PATH}/test-checksummed_array-faults)

    add_executable(test-vector-chunk libs/self_healing/test/vector-chunk.cpp)
    add_test(vector-chunk ${EXECUTABLE_OUTPUT_PATH}/test-vector-chunk)
    add_executable(test-vector libs/self_healing/test/vector.cpp)
    add_test(vector ${EXECUTABLE_OUTPUT_PATH}/test-vector)
    add_executable(test-vector-iterators libs/self_healing/test/vector-iterators.cpp)
    add_test(vector-iterators ${EXECUTABLE_OUTPUT_PATH}/test-vector-iterators)
    add_executable(test-vector-faults libs/self_healing/test/vector-faults.cpp)
    add_test(vector-faults ${EXECUTABLE_OUTPUT_PATH}/test-vector-faults)
endif()

# installation
set(DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/boost/self_healing")
install(DIRECTORY boost DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.hpp")
install(FILES AUTHORS.txt COPYING.txt LICENSE_1_0.txt README.txt TODO.txt DESTINATION ${DOC_INSTALL_DIR})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/ DESTINATION ${DOC_INSTALL_DIR} OPTIONAL)
