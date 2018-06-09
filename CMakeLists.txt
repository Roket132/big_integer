cmake_minimum_required(VERSION 3.10)

project(BIGINT)

set(CMAKE_CXX_STANDARD 17)

include_directories(${BIGINT_SOURCE_DIR})

add_executable(big_integer_testing
               big_integer_testing.cpp
               big_integer.h
               big_integer.cpp
               gtest/gtest-all.cc
               gtest/gtest.h
               gtest/gtest_main.cc
        MyTests.cpp)

