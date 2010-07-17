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

# compile-time options
option(FIXING_CHECKS "Data structure checks also fix issues" on)
option(BUILD_TESTS "Build tests" on)

if(FIXING_CHECKS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_SELF_HEALING_FIXING_CHECKS")
endif()

# dependency checks
find_package(Boost 1.42.0 REQUIRED)
find_package(Doxygen)
find_package(Docutils)

include_directories(${Boost_INCLUDE_DIRS})

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

    # tests related to utility class safe_ref
    add_executable(test-safe-ref libs/self_healing/test/safe_ref.cpp)
    add_test(safe-ref ${EXECUTABLE_OUTPUT_PATH}/test-safe-ref)

    # tests related to class checksummed array
    add_executable(test-array libs/self_healing/test/array.cpp)
    add_test(array ${EXECUTABLE_OUTPUT_PATH}/test-array)
    add_executable(test-array-iterators libs/self_healing/test/array-iterators.cpp)
    add_test(array-iterators ${EXECUTABLE_OUTPUT_PATH}/test-array-iterators)
    add_executable(test-array-faults libs/self_healing/test/array-faults.cpp)
    add_test(array-faults ${EXECUTABLE_OUTPUT_PATH}/test-array-faults)

    # tests related to class vector
    add_executable(test-vector libs/self_healing/test/vector.cpp)
    add_test(vector ${EXECUTABLE_OUTPUT_PATH}/test-vector)
    add_executable(test-vector-iterators libs/self_healing/test/vector-iterators.cpp)
    add_test(vector-iterators ${EXECUTABLE_OUTPUT_PATH}/test-vector-iterators)
    add_executable(test-vector-faults libs/self_healing/test/vector-faults.cpp)
    add_test(vector-faults ${EXECUTABLE_OUTPUT_PATH}/test-vector-faults)

    # tests related to class multiset helper classes multiset_node and multiset_leaf
    add_executable(test-multiset-node libs/self_healing/test/multiset-node.cpp)
    add_test(multiset-node ${EXECUTABLE_OUTPUT_PATH}/test-multiset-node)
    add_executable(test-multiset-leaf libs/self_healing/test/multiset-leaf.cpp)
    add_test(multiset-leaf ${EXECUTABLE_OUTPUT_PATH}/test-multiset-leaf)

    # tests related to class multiset
    #add_executable(test-multiset libs/self_healing/test/multiset.cpp)
    #add_test(multiset ${EXECUTABLE_OUTPUT_PATH}/test-multiset)
    #add_executable(test-multiset-iterators libs/self_healing/test/multiset-iterators.cpp)
    #add_test(multiset-iterators ${EXECUTABLE_OUTPUT_PATH}/test-multiset-iterators)
    add_executable(test-multiset-faults libs/self_healing/test/multiset-faults.cpp)
    add_test(multiset-faults ${EXECUTABLE_OUTPUT_PATH}/test-multiset-faults)
endif()

# installation
set(DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/boost/self_healing")
install(DIRECTORY boost DESTINATION ${CMAKE_INSTALL_PREFIX}/include FILES_MATCHING PATTERN "*.hpp")
install(FILES AUTHORS.txt COPYING.txt LICENSE_1_0.txt README.txt TODO.txt DESTINATION ${DOC_INSTALL_DIR})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/ DESTINATION ${DOC_INSTALL_DIR} OPTIONAL)
