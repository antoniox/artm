#include <fstream>
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
        ("topics_count", po::value<size_type>(), "number of topics used for processing")
        ("inner_iterations", po::value<size_type>(), "number of inner iterations")
        ("batch_size", po::value<size_type>(), "batch size");
}


int main(int argc, const char ** argv) {
    init_logging(argv[0]);

    auto options = parse_options(&set_options, argc, argv);
    LOG_INFO << options;

    std::ifstream vocabulary_stream(options["vocabulary"].as<std::string>());

    Vocabulary vocabulary;
    vocabulary.load(vocabulary_stream);

    size_type topics_count = options["topics_count"].as<size_type>();

    size_type inner_iterations = options["inner_iterations"].as<size_type>();

    std::ifstream corpus_stream(options["corpus"].as<std::string>());

    size_type batch_size = options["batch_size"].as<size_type>();
    BatchParser parser(corpus_stream, batch_size);

    Batch chunk;

    BatchProcessor processor(topics_count, inner_iterations, vocabulary);

    size_type doc_id = 0;

    while (parser.has_more()) {
        parser.parse(chunk);
        processor.process(chunk);

        LOG_INFO_EVERY_N(10, "chunks");
    }

    std::ofstream model_stream(options["model"].as<std::string>());
    processor.save_model(model_stream);

    return 0;
}
