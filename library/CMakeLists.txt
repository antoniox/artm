project(artm_library CXX)

set(ARTM_INCLUDE_DIRS
    ${PCRECPP_INCLUDE_DIRS}
    ${BOOST_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE PATH
    "artm include dirs"
)

add_subdirectory(src)
