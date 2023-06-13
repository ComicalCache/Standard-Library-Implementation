cmake_minimum_required(VERSION 3.20)
project(Heap)

set(CMAKE_CXX_STANDARD 23)

include_directories(${CMAKE_SOURCE_DIR}/Data_Structures/)

add_library(Heap Heap.cpp Heap.hpp)