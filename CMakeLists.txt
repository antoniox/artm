cmake_minimum_required (VERSION 2.8.7)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

project (artm CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")

message(STATUS "Build type is ${CMAKE_BUILD_TYPE}")

find_package(Boost
    COMPONENTS 
        program_options iostreams
    REQUIRED
)

find_package(PCRECPP)
find_package(GLOG)

add_subdirectory(library)
add_subdirectory(tools)
