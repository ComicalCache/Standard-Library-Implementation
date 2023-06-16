cmake_minimum_required(VERSION 3.20)
project(Testing)

set(CMAKE_CXX_STANDARD 23)

# Boost
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(BOOST_VER 1_81_0)
set(BOOST_PATH /opt/homebrew/Cellar/boost/1.81.0_1)
set(Boost_INCLUDE_DIRS ${BOOST_PATH}/include)
set(Boost_LIBRARIES ${BOOST_PATH}/lib)

if ((EXISTS "${Boost_INCLUDE_DIRS}" AND IS_DIRECTORY "${Boost_INCLUDE_DIRS}")
        AND (EXISTS "${Boost_LIBRARIES}" AND IS_DIRECTORY "${Boost_LIBRARIES}"))
    include_directories(${Boost_INCLUDE_DIRS})

    # Test Object Include
    include_directories(Test_Object)

    # Implementation Include Data Structures
    include_directories(${CMAKE_SOURCE_DIR}/Data_Structures/)

    # Test Cases
    add_subdirectory(Data_Structures/DynamicArray)
    add_subdirectory(Data_Structures/StaticArray)
    add_subdirectory(Data_Structures/Vector)
    add_subdirectory(Data_Structures/Heap)
else ()
    message(
            "Error locating boost library version ${BOOST_VER}, solutions:
  - install boost version ${BOOST_VER} to ${BOOST_PATH}/boost_${BOOST_VER}
  - change the BOOST_VER and BOOST_PATH variables accordingly"
    )
endif ()
