#pragma once

#include <fstream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

void skip_line(std::istream & input);

po::variables_map parse_options(
    void (*set_options)(po::options_description &),
    int argc, const char ** argv
);
