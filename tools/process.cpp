#include <fstream>
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
        ("corpus-directory", po::value<std::string>(), "corpus directory path")
        ("vocabulary-directory", po::value<std::string>(), "vocabulary directory path")
        ("model-directory", po::value<std::string>(), "model directory path")
        ("corpus", po::value<std::string>(), "processed corpus filename")
        ("vocabulary", po::value<std::string>(), "vocabulary filename")
        ("model", po::value<std::string>(), "model filename")
        ("input-suffix", po::value<std::string>(), "suffix of data to be used")
        ("suffix", po::value<std::string>(), "suffix of model to be built")
        ("topics-count", po::value<size_type>(), "number of topics used for processing")
        ("inner-iterations", po::value<size_type>(), "number of inner iterations")
        ("batch-size", po::value<size_type>(), "batch size");
}


int main(int argc, const char ** argv) {
    auto options = parse_options(&set_options, argc, argv);

    init_logging(argv[0], options);
    LOG_INFO << "Running " << argv[0] << "..." <<  std::endl;
    LOG_INFO << options;

    auto input_suffix = options["input-suffix"].as<std::string>();
    auto suffix = options["suffix"].as<std::string>();

    auto corpus_directory = options["corpus-directory"].as<std::string>();

    auto corpus_filename = options["corpus"].as<std::string>();
    corpus_filename = corpus_directory + "/" + corpus_filename + input_suffix;

    auto vocabulary_directory = options["vocabulary-directory"].as<std::string>();
    auto vocabulary_filename = options["vocabulary"].as<std::string>();
    vocabulary_filename = vocabulary_directory + "/" + vocabulary_filename + input_suffix;

    auto model_directory = options["model-directory"].as<std::string>();
    auto model_filename = options["model"].as<std::string>();
    model_filename = model_directory + "/" + model_filename + suffix;

    std::ifstream vocabulary_stream(vocabulary_filename);

    Vocabulary vocabulary;
    vocabulary.load(vocabulary_stream);

    size_type topics_count = options["topics-count"].as<size_type>();
    size_type inner_iterations = options["inner-iterations"].as<size_type>();

    std::ifstream corpus_stream(corpus_filename);

    size_type batch_size = options["batch-size"].as<size_type>();
    BatchParser parser(corpus_stream, batch_size);

    Batch chunk;
    BatchProcessor processor(
        topics_count, inner_iterations, vocabulary.modality_sizes()
    );

    size_t count = 0;

    while (parser.has_more()) {
        // if (count >= 30) break;
        parser.parse(chunk);
        processor.process(chunk);

        count += 1;

        LOG_INFO_EVERY_N(10, "chunks");
    }

    std::ofstream model_stream(model_filename);
    processor.save_model(model_stream);

    return 0;
}
