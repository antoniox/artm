#include <cstdlib>
#include <iostream>
#include <string>

#include "utils.h"


void skip_line(std::istream & input) {
    static std::string unused;
    std::getline(input, unused);
}


po::variables_map parse_options(
    void (*set_options)(po::options_description &),
    int argc, const char ** argv
) {
    po::options_description description;

    description.add_options()
        ("help", "produce help message");

    (*set_options)(description);

    po::variables_map options;
    po::store(
        po::command_line_parser(argc, argv)
            .options(description)
            .style(
                po::command_line_style::unix_style |
                po::command_line_style::allow_guessing |
                po::command_line_style::allow_long_disguise
            )
            .run(),
        options
    );

    po::notify(options);

    if (options.count("help")) {
        std::cout << description << std::endl;
        std::exit(1);
    }

    return options;
}
