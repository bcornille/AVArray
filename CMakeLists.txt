cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)
project(AVArray VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(test)