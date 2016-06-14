#pragma once

#include <fstream>

#include <boost/program_options.hpp>

#include "utils/types.h"

namespace po = boost::program_options;

void skip_line(std::istream & input);

po::variables_map parse_options(
    void (*set_options)(po::options_description &),
    int argc, const char ** argv
);

std::ostream & operator << (
    std::ostream & output, 
    const po::variables_map & options
);


float_type uniform_random();
float_type zero();


#define FOR(name, limit) \
    for (id_type name = 0; name < limit; ++name)

#define PARALLEL_FOR(name, limit) \
    _Pragma("omp parallel for") \
    FOR(name, limit)

//_Pragma("omp parallel for") \


template <typename T>
void write_binary(std::ostream & output, const T & value);

template <typename T>
void read_binary(std::istream & input, T & value);
