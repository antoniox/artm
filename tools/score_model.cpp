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
        ("corpus-directory", po::value<std::string>(), "corpus directory path")
        ("vocabulary-directory", po::value<std::string>(), "vocabulary directory path")
        ("model-directory", po::value<std::string>(), "model directory path")
        ("vocabulary", po::value<std::string>(), "vocabulary filename")
        ("model", po::value<std::string>(), "model filename")
        ("suffix", po::value<std::string>(), "suffix of vocabulary to be used")
        ("model-suffix", po::value<std::string>(), "suffix of model to be used")
        ("topics-count", po::value<size_type>(), "number of topics used for processing")
}


int main(int argc, const char ** argv) {
    auto options = parse_options(&set_options, argc, argv);

    init_logging(argv[0], options);
    LOG_INFO << "Running " << argv[0] << "..." <<  std::endl;
    LOG_INFO << options;

    auto vocabulary_suffix = options["vocabulary-suffix"].as<std::string>();
    auto model_suffix = options["model-suffix"].as<std::string>();
    model_suffix = vocabulary_suffix + model_suffix;

    auto vocabulary_directory = options["vocabulary-directory"].as<std::string>();
    auto vocabulary_filename = options["vocabulary"].as<std::string>();
    vocabulary_filename = vocabulary_directory + "/" + vocabulary_filename + vocabulary_suffix;

    auto model_directory = options["model-directory"].as<std::string>();
    auto model_filename = options["model"].as<std::string>();
    model_filename = model_directory + "/" + model_filename + model_suffix;

    std::ifstream vocabulary_stream(vocabulary_filename);
    std::ifstream model_stream(model_filename);

    Vocabulary vocabulary;
    vocabulary.load(vocabulary_stream);

    size_type topics_count = options["topics-count"].as<size_type>();

    Phi phi(topics_count, vocabulary.modality_sizes(), &zero);
    phi.load(model_stream);

    size_type top_words = options["top-words"].as<size_type>();
    size_type top_nicks = options["top-nicks"].as<size_type>();
    size_type top_dates = options["top-dates"].as<size_type>();

    TopTokens top(topics_count, {top_words, top_nicks, top_dates});
    top.fill_from(phi);

    top.print(vocabulary, std::cout);

    return 0;
}
