#include <fstream>
#include <iostream>
#include <string>

#include "vocabulary.h"
#include "phi.h"
#include "phi_record.h"
#include "top_tokens.h"
#include "utils.h"


void set_options(po::options_description & description) {
    description.add_options()
        ("config",
            po::value<std::string>()->default_value("print_top.cfg"),
            "config filename")
        ("vocabulary-directory", po::value<std::string>(), "vocabulary directory path")
        ("model-directory", po::value<std::string>(), "model directory path")
        ("vocabulary", po::value<std::string>(), "vocabulary filename")
        ("model", po::value<std::string>(), "model filename")
        ("suffix", po::value<std::string>(), "suffix of data to be used")
        ("topics_count", po::value<size_type>(), "number of topics used for processing")
        ("top_words", po::value<size_type>(), "number of top words to print")
        ("top_nicks", po::value<size_type>(), "number of top nicks to print")
        ("top_dates", po::value<size_type>(), "number of top dates to print");
}


int main(int argc, const char ** argv) {
    auto options = parse_options(&set_options, argc, argv);

    init_logging(argv[0], options);
    LOG_INFO << "Running " << argv[0] << "..." <<  std::endl;
    LOG_INFO << options;

    auto suffix = options["suffix"].as<std::string>();

    auto vocabulary_directory = options["vocabulary-directory"].as<std::string>();
    auto vocabulary_filename = options["vocabulary"].as<std::string>();
    vocabulary_filename = vocabulary_directory + "/" + vocabulary_filename + suffix;

    auto model_directory = options["model-directory"].as<std::string>();
    auto model_filename = options["model"].as<std::string>();
    model_filename = model_directory + "/" + model_filename + suffix;

    std::ifstream vocabulary_stream(vocabulary_filename);
    std::ifstream model_stream(model_filename);

    Vocabulary vocabulary;
    vocabulary.load(vocabulary_stream);

    size_type topics_count = options["topics_count"].as<size_type>();

    Phi phi(topics_count, vocabulary.modality_sizes(), &zero);
    phi.load(model_stream);

    size_type top_words = options["top_words"].as<size_type>();
    size_type top_nicks = options["top_nicks"].as<size_type>();
    size_type top_dates = options["top_dates"].as<size_type>();

    TopTokens top(topics_count, {top_words, top_nicks, top_dates});
    top.fill_from(phi);

    top.print(vocabulary, std::cout);

    return 0;
}
