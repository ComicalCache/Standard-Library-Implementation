cmake_minimum_required(VERSION 3.20)
project(StaticArray)

set(CMAKE_CXX_STANDARD 23)

add_library(StaticArray StaticArray.cpp StaticArray.hpp)
