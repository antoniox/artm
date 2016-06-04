cmake_minimum_required (VERSION 2.8.7)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/)

project (artm CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fopenmp")

# Update built-in build types
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} -g")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")
set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_DEBUG} -pg")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

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
