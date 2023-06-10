cmake_minimum_required(VERSION 3.17)
project(Cpp_std_lib)

set(CMAKE_CXX_STANDARD 14)

# Implementations
add_subdirectory(Data_Structures/Array)
add_subdirectory(Data_Structures/Heap)
add_subdirectory(Data_Structures/Linked_List)
add_subdirectory(Data_Structures/Vector)

# Testing
add_subdirectory(Testing)

# Manual Testing
set(MT_DIR Testing/Manual_Testing)
if(EXISTS "${MT_DIR}" AND IS_DIRECTORY "${MT_DIR}")
    add_subdirectory(${MT_DIR})
else()
    message("Testing/Manual_Testing not found, building without...")
endif()
