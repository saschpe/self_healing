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
include_directories(${Boost_INCLUDE_DIRS})

option(BOOST_SELF_HEALING_BUILD_TESTS "Build the tests" on)
set(BOOST_SELF_HEALING_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/")
set(BOOST_SELF_HEALING_LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/")
set(BOOST_SELF_HEALING_DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/self_healing")

set(self_healing_HDRS
    boost/self_healing/detail/nullary_function.hpp
    boost/self_healing/checksummed_array.hpp
    boost/self_healing/reference.hpp
    boost/self_healing/vector.hpp
    boost/self_healing.hpp
)

set(self_healing_LIB_SRCS
)

if(BOOST_SELF_HEALING_BUILD_TESTS)
    enable_testing()
    include_directories(.) # to allow global #include

    add_executable(test-reference-base libs/self_healing/test/reference-base.cpp)
    add_test(reference-base ${EXECUTABLE_OUTPUT_PATH}/test-reference-base)

    add_executable(test-checksummed_array-base libs/self_healing/test/checksummed_array-base.cpp)
    add_test(checksummed_array-base ${EXECUTABLE_OUTPUT_PATH}/test-checksummed_array-base)
    
    add_executable(test-checksummed_array-iterators libs/self_healing/test/checksummed_array-iterators.cpp)
    add_test(checksummed_array-iterators ${EXECUTABLE_OUTPUT_PATH}/test-checksummed_array-iterators)
    
    add_executable(test-checksummed_array-faults libs/self_healing/test/checksummed_array-faults.cpp)
    add_test(checksummed_array-faults ${EXECUTABLE_OUTPUT_PATH}/test-checksummed_array-faults)
    
    #add_executable(test-vector-base libs/self_healing/test/vector-base.cpp)
    #add_test(vector-base ${EXECUTABLE_OUTPUT_PATH}/test-vector-base)
    
    #add_executable(test-vector-faults libs/self_healing/test/vector-faults.cpp)
    #add_test(vector-faults ${EXECUTABLE_OUTPUT_PATH}/test-vector-faults)
endif(BOOST_SELF_HEALING_BUILD_TESTS)

# Installation
#if(WIN32 AND NOT UNIX)
    #install(DIRECTORY docs/ DESTINATION ${CMAKE_INSTALL_PREFIX}/docs)
    #install(FILES AUTHORS.txt README.txt LICENSE.txt DESTINATION ${CMAKE_INSTALL_PREFIX})
#else()
    #install(FILES ${self_healing_HDRS} DESTINATION ${BOOST_SELF_HEALING_INCLUDE_INSTALL_DIR}/self_healing COMPONENT Devel)
    #install(FILES AUTHORS LICENSE README DESTINATION ${CMAKE_INSTALL_PREFIX})
    #install(DIRECTORY docs/ DESTINATION share/doc/masonry)
    #install(FILES AUTHORS.txt README.txt LICENSE.txt DESTINATION share/doc/masonry)
#endif()
