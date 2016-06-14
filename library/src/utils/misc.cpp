#include <iostream>
#include <fstream>
#include <random>

#include <string>

#include "utils/misc.h"


void skip_line(std::istream & input) {
    static std::string unused;
    std::getline(input, unused);
}


po::variables_map parse_options(
    void (*set_options)(po::options_description &),
    int argc, const char ** argv
) {
    po::options_description description;
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


float_type uniform_random() {
    static std::random_device random_device;
    static std::mt19937 random_generator(random_device());
    static std::uniform_real_distribution<float_type> distribution(0, 1);

    return distribution(random_generator);
}


float_type zero() {
    return 0;
}


template <typename T>
void write_binary(std::ostream & output, const T & value) {
    output.write(reinterpret_cast<const char *>(&value), sizeof(T));
}


template <>
void write_binary(std::ostream & output, const std::string & value) {
    size_type size = value.length() + 1;
    write_binary(output, size);
    output.write(value.c_str(), size);
}


template <typename T>
void read_binary(std::istream & input, T & value) {
    input.read(reinterpret_cast<char *>(&value), sizeof(T));
}

template <>
void read_binary(std::istream & input, std::string & value) {
    size_type size;
    read_binary(input, size);

    char * tmp = new char [size];
    input.read(tmp, size);
    value = tmp;
    delete [] tmp;
}
