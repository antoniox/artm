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
        ("topics_count", po::value<size_type>(), "number of topics used for processing")
        ("top_words", po::value<size_type>(), "number of top words to print")
        ("top_nicks", po::value<size_type>(), "number of top nicks to print")
        ("top_dates", po::value<size_type>(), "number of top dates to print");
}


int main(int argc, const char ** argv) {
    init_logging(argv[0]);
    LOG_INFO << "Running " << argv[0] << "..." <<  std::endl;

    auto options = parse_options(&set_options, argc, argv);
    LOG_INFO << options;

    std::ifstream vocabulary_stream(options["vocabulary"].as<std::string>());
    std::ifstream model_stream(options["model"].as<std::string>());

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
