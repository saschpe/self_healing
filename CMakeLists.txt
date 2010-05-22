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
set(CMAKE_BUILD_TYPE Debug)

find_package(Boost 1.42.0 REQUIRED)
find_package(Doxygen)

include_directories(${Boost_INCLUDE_DIRS})

option(BUILD_TESTS "Build tests" on)

# add doxygen source code documentation as target
if(DOXYGEN_FOUND)
    #TODO: This is not platform independent!
    # add doxygen as target
    add_custom_target(doxygen COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/doc
                              COMMAND ${DOXYGEN_EXECUTABLE} reference.dox
                              WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/self_healing/doc/
                              COMMENT "Generating code documentation")
    # cleanup $build/doc on "make clean"
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_CURRENT_SOURCE_DIR}/doc)
    # add doxygen as dependency to doc-target
    get_target_property(DOC_TARGET doc TYPE)
    if(NOT DOC_TARGET)
        add_custom_target(doc)
    endif()
    add_dependencies(doc doxygen)
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
    #add_executable(test-vector libs/self_healing/test/vector.cpp)
    #add_test(vector ${EXECUTABLE_OUTPUT_PATH}/test-vector)
    #add_executable(test-vector-iterators libs/self_healing/test/vector-iterators.cpp)
    #add_test(vector-iterators ${EXECUTABLE_OUTPUT_PATH}/test-vector-iterators)
    add_executable(test-vector-faults libs/self_healing/test/vector-faults.cpp)
    add_test(vector-faults ${EXECUTABLE_OUTPUT_PATH}/test-vector-faults)
endif()

# installation
set(DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/boost/self_healing")
install(DIRECTORY boost DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.hpp")
install(FILES AUTHORS.txt COPYING.txt LICENSE_1_0.txt README.txt TODO.txt DESTINATION ${DOC_INSTALL_DIR})
install(DIRECTORY doc/ DESTINATION ${DOC_INSTALL_DIR} OPTIONAL)
