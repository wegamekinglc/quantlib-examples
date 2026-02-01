cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)

project(QuantLib-Examples)

include_directories("/home/wegamekinglc/dev/github/Engine/include")
link_directories("/home/wegamekinglc/dev/github/Engine/lib")
add_subdirectory("CurveBootstrap")