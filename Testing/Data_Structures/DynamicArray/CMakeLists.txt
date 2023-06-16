cmake_minimum_required(VERSION 3.20)
project(DynamicArray_Test)

set(CMAKE_CXX_STANDARD 23)

add_executable(DynamicArray_Test DynamicArray_Test.cpp)
target_link_libraries(DynamicArray_Test ${Boost_LIBRARIES})

