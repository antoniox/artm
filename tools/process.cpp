#include <iostream>
#include <string>

#include "batch.h"
#include "batch_processor.h"
#include "utils.h"
#include "vocabulary.h"


void set_options(po::options_description & description) {
    description.add_options()
        ("config",
            po::value<std::string>()->default_value("process.cfg"),
            "config filename")
        ("corpus", po::value<std::string>(), "corpus filename")
        ("vocabulary", po::value<std::string>(), "vocabulary filename")
        ("model", po::value<std::string>(), "model filename")
        ("suffix", po::value<std::string>(), "suffix of input data")
        ("topics-count", po::value<size_type>(), "number of topics used for processing")
        ("inner-iterations", po::value<size_type>(), "number of inner iterations")
        ("batch-size", po::value<size_type>(), "batch size");
}


int main(int argc, const char ** argv) {
    auto options = parse_options(&set_options, argc, argv);

    init_logging(argv[0], options);
    LOG_INFO << "Running " << argv[0] << "..." <<  std::endl;
    LOG_INFO << options;

    size_type topics_count = options["topics-count"].as<size_type>();
    size_type inner_iterations = options["inner-iterations"].as<size_type>();
    size_type batch_size = options["batch-size"].as<size_type>();

    auto suffix = options["suffix"].as<std::string>();

    std::string model_suffix = suffix + "." +
        std::to_string(topics_count) + "_" +
        std::to_string(inner_iterations) + "_" + 
        std::to_string(batch_size);

    auto corpus_filename = options["corpus"].as<std::string>();
    corpus_filename = corpus_filename + suffix;

    auto vocabulary_filename = options["vocabulary"].as<std::string>();
    vocabulary_filename = vocabulary_filename + suffix;

    auto model_filename = options["model"].as<std::string>();
    model_filename = model_filename + model_suffix;

    Vocabulary vocabulary(vocabulary_filename);

    std::ifstream corpus_stream(corpus_filename);

    Phi phi(topics_count, vocabulary.modality_sizes(), &uniform_random);
    Phi phi_counters = phi;
    phi_counters.normalize_into(phi);

    BatchParser parser(corpus_stream, batch_size);

    Batch chunk;
    BatchProcessor processor;

    size_t counter = 0;

    while (parser.has_more()) {
        // if (counter > 10) break;
        parser.parse(chunk);
        processor.process(chunk, phi, phi_counters, inner_iterations);

        counter++;
    }

    phi.save(model_filename);

    return 0;
}
