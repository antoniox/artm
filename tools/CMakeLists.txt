include_directories(
    ${ARTM_INCLUDE_DIRS}
)

link_libraries(
    artm
    ${Boost_LIBRARIES}
    ${GLOG_LIBRARIES}
)

add_executable(parse_corpus parse_corpus.cpp)
add_executable(shrink_corpus shrink_corpus.cpp)
add_executable(process process.cpp)
