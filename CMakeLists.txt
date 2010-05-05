#
# (C) Copyright Sascha Peilicke 2010.
#
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
project(robust)
cmake_minimum_required(VERSION 2.6.4)

set(CMAKE_MODULE_PATH "${robust_SOURCE_DIR}/cmake/modules")
#
find_package(Boost 1.42.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

option(ROBUST_BUILD_TESTS "Build the tests" on)
set(ROBUST_INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/")
set(ROBUST_LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/")
set(ROBUST_DOC_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/robust")

set(robust_HDRS
    boost/robust/array.hpp
    boost/robust/functor.hpp
    boost/robust/reference.hpp
    boost/robust/vector.hpp
    boost/robust.hpp
)

set(robust_LIB_SRCS
)

if(ROBUST_BUILD_TESTS)
    enable_testing()
    include_directories(.) # to allow global #include
    add_executable(test-array-base libs/robust/test/array-base.cpp)
    add_test(array-base ${EXECUTABLE_OUTPUT_PATH}/test-array-base)
    add_executable(test-array-faults libs/robust/test/array-faults.cpp)
    add_test(array-faults ${EXECUTABLE_OUTPUT_PATH}/test-array-faults)
    add_executable(test-vector-base libs/robust/test/vector-base.cpp)
    add_test(vector-base ${EXECUTABLE_OUTPUT_PATH}/test-vector-base)
    add_executable(test-vector-faults libs/robust/test/vector-faults.cpp)
    add_test(vector-faults ${EXECUTABLE_OUTPUT_PATH}/test-vector-faults)
endif(ROBUST_BUILD_TESTS)

# Installation
#if(WIN32 AND NOT UNIX)
    #install(DIRECTORY docs/ DESTINATION ${CMAKE_INSTALL_PREFIX}/docs)
    #install(FILES AUTHORS.txt README.txt LICENSE.txt DESTINATION ${CMAKE_INSTALL_PREFIX})
#else()
    #install(FILES ${robust_HDRS} DESTINATION ${ROBUST_INCLUDE_INSTALL_DIR}/robust COMPONENT Devel)
    #install(FILES AUTHORS LICENSE README DESTINATION ${CMAKE_INSTALL_PREFIX})
    #install(DIRECTORY docs/ DESTINATION share/doc/masonry)
    #install(FILES AUTHORS.txt README.txt LICENSE.txt DESTINATION share/doc/masonry)
#endif()
