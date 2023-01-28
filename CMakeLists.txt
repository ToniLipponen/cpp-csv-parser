cmake_minimum_required(VERSION 3.24)
project(test)

set(CMAKE_CXX_STANDARD 11)

add_executable(test test.cpp)

add_custom_command(
        TARGET test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/test_dataset.csv
        ${CMAKE_CURRENT_BINARY_DIR}/test_dataset.csv)