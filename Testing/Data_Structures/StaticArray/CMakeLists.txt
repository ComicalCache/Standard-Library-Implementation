cmake_minimum_required(VERSION 3.20)
project(StaticArray_Test)

set(CMAKE_CXX_STANDARD 23)

add_executable(StaticArray_Test StaticArray_Test.cpp)
target_link_libraries(StaticArray_Test ${Boost_LIBRARIES})

