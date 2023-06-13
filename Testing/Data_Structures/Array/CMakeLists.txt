cmake_minimum_required(VERSION 3.20)
project(Array_Test)

set(CMAKE_CXX_STANDARD 23)

add_executable(Array_Test Array_Test.cpp)
target_link_libraries(Array_Test ${Boost_LIBRARIES})

