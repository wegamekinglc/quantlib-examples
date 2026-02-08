cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)

project(QuantLib-Examples)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(Platform)

find_package(Boost 1.71 REQUIRED COMPONENTS thread)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "Boost not found!")
endif()

set(QuantLib_DIR "D:/dev/github/Engine/lib/cmake/QuantLib")
find_package(QuantLib REQUIRED)

add_subdirectory("CurveBootstrap")