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
add_executable(convert_vocabulary convert_vocabulary.cpp)
add_executable(convert_corpus convert_corpus.cpp)
add_executable(convert_phi convert_phi.cpp)
add_executable(print_top print_top.cpp)
