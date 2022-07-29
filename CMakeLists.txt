cmake_minimum_required(VERSION 3.10)
project(complex VERSION "0.0.0")
set(CMAKE_CXX_STANDARD 17)
cmake_policy(SET CMP0077 NEW)

add_library(complex INTERFACE)
target_include_directories(complex INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
