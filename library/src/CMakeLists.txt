include_directories(${ARTM_INCLUDE_DIRS})

FILE(GLOB CPP_FILES *.cpp)

add_library(artm STATIC
    ${CPP_FILES}
)

target_link_libraries(artm
    ${PCRECPP_LIBRARIES}
    ${Boost_LIBRARIES}
)
