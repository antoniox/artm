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
        ("vocabulary", po::value<std::string>(), "vocabulary filename")
        ("model", po::value<std::string>(), "model filename")
        ("vocabulary-suffix", po::value<std::string>(), "suffix of vocabulary to be used")
        ("model-suffix", po::value<std::string>(), "suffix of model to be used")
        ("top-words", po::value<size_type>(), "number of top words to print")
        ("top-nicks", po::value<size_type>(), "number of top nicks to print")
        ("top-dates", po::value<size_type>(), "number of top dates to print");
}


int main(int argc, const char ** argv) {
    auto options = parse_options(&set_options, argc, argv);

    init_logging(argv[0], options);
    LOG_INFO << "Running " << argv[0] << "..." <<  std::endl;
    LOG_INFO << options;

    std::string vocabulary_suffix = options["vocabulary-suffix"].as<std::string>();
    std::string model_suffix = options["model-suffix"].as<std::string>();
    model_suffix = vocabulary_suffix + model_suffix;

    std::string vocabulary_filename = options["vocabulary"].as<std::string>();
    vocabulary_filename = vocabulary_filename + vocabulary_suffix;

    std::string model_filename = options["model"].as<std::string>();
    model_filename = model_filename + model_suffix;

    Vocabulary vocabulary(vocabulary_filename);
    Phi phi(model_filename);

    size_type top_words = options["top-words"].as<size_type>();
    size_type top_nicks = options["top-nicks"].as<size_type>();
    size_type top_dates = options["top-dates"].as<size_type>();

    TopTokens top(phi.topics_count_, {top_words, top_nicks, top_dates});
    top.fill_from(phi);

    top.print(vocabulary, std::cout);

    return 0;
}
