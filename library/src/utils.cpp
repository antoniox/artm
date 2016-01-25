#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include <unistd.h>

#include "typedefs.h"
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
                po::command_line_style::unix_style
            )
            .run(),
        options
    );
    po::notify(options);

    std::ifstream config(options["config"].as<std::string>());

    po::store(
        po::parse_config_file(config, description),
        options
    );
    po::notify(options);

    if (options.count("help")) {
        std::cout << description << std::endl;
        std::exit(1);
    }

    return options;
}


std::ostream & operator << (
    std::ostream & output, 
    const po::variables_map & options
) {
    output << "Options:" << std::endl;

    for (auto & pair : options) {
        output << "\t" << pair.first << " -> ";

        auto & value = pair.second.value();

        if (value.type() == typeid(size_type)) {
            output << boost::any_cast<size_type>(value);
        } else if (value.type() == typeid(std::string)) {
            output << boost::any_cast<std::string>(value);
        }

        output << std::endl;
    }
}


void init_logging(const char * program_name) {
    dup2(STDOUT_FILENO, STDERR_FILENO);
    FLAGS_logtostderr = 1;
    google::InitGoogleLogging(program_name);
}
