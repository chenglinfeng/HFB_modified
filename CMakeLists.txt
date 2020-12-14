# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.

option(module_test "module_test" ON)
option(module_build_shared "module_build_shared" ON)
option(module_build_static "module_build_static" ON)

cmake_minimum_required(VERSION 3.10)

set(module_name HFB)

project(${module_name} VERSION 1.1.0 LANGUAGES CXX)

message(STATUS "Configuring ${module_name}")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
     message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lmorseot -O3 -fPIC")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -W -Wwrite-strings -Werror -O3")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Import Microsoft SEAL
find_package(SEAL 3.3.0 EXACT REQUIRED)

# head
include_directories(${PROJECT_SOURCE_DIR}/include)
add_subdirectory(${PROJECT_SOURCE_DIR}/include)

# src
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

# test
# IF(module_test)
#     add_subdirectory(${PROJECT_SOURCE_DIR}/test)
# ENDIF(module_test)

add_subdirectory(${PROJECT_SOURCE_DIR}/dectree)

add_subdirectory(${PROJECT_SOURCE_DIR}/PPDGI)
