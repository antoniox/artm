#include <iostream>
#include <fstream>
#include <random>
#include <string>

#include <fcntl.h>
#include <unistd.h>

#include "types.h"
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
        ("log-directory",
            po::value<std::string>()->default_value("logs"),
            "log directory path")
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


void init_logging(const char * program_name, const po::variables_map & options) {
    auto log_directory = options["log-directory"].as<std::string>();

    std::string log_filename(
        program_name + 2, program_name + strlen(program_name)
    );

    log_filename = log_directory + "/" + log_filename;

    if (options.count("suffix")) {
        auto suffix = options["suffix"].as<std::string>();
        log_filename += suffix;
    }

    log_filename += ".log";

    int log_fd = open(log_filename.c_str(), O_WRONLY | O_CREAT, 0644);

    dup2(log_fd, STDERR_FILENO);
    FLAGS_logtostderr = 1;

    google::InitGoogleLogging(program_name + 2);
}


float_type uniform_random() {
    static std::random_device random_device;
    static std::mt19937 random_generator(random_device());
    static std::uniform_real_distribution<float_type> distribution(0, 1);

    return distribution(random_generator);
}


float_type zero() {
    return 0;
}
