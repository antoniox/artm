include_directories(${ARTM_INCLUDE_DIRS})

FILE(GLOB_RECURSE CPP_FILES *.cpp)

add_library(artm STATIC
    ${CPP_FILES}
)

target_link_libraries(artm
    ${PCRECPP_LIBRARIES}
    ${GLOG_LIBRARIES}
    ${Boost_LIBRARIES}
)
