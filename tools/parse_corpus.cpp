#include <fstream>

#include "document_record.h"
#include "vocabulary.h"
#include "raw_record.h"
#include "utils.h"


void set_options(po::options_description & description) {
    description.add_options()
        ("config",
            po::value<std::string>()->default_value("parse_corpus.cfg"),
            "config filename")
        ("raw-corpus", po::value<std::string>(), "raw corpus filename")
        ("corpus", po::value<std::string>(), "processed corpus filename")
        ("vocabulary", po::value<std::string>(), "vocabulary filename");
}


int main(int argc, const char ** argv) {
    init_logging(argv[0]);
    LOG_INFO << "Running " << argv[0] << "..." <<  std::endl;

    auto options = parse_options(&set_options, argc, argv);
    LOG_INFO << options;

    std::ifstream raw_corpus_stream(options["raw-corpus"].as<std::string>());
    std::ofstream corpus_stream(options["corpus"].as<std::string>(), std::ios::binary);
    std::ofstream vocabulary_stream(options["vocabulary"].as<std::string>(), std::ios::binary);

    RawRecord raw_record;
    Vocabulary vocabulary;

    // skip header
    skip_line(raw_corpus_stream);

    while (raw_record.load(raw_corpus_stream)) {
        auto tokenizer = raw_record.tokenize(vocabulary);

        for (auto & document_record : tokenizer) {
            document_record.save(corpus_stream);
        }

        LOG_INFO_EVERY_N(10000, "documents");
    }

    vocabulary.save(vocabulary_stream);

    LOG_INFO << "Token statistics: \n" << 
        "\twords: " << vocabulary.modality_size(WORD) << std::endl <<
        "\tnicks: " << vocabulary.modality_size(NICK) << std::endl <<
        "\tdates: " << vocabulary.modality_size(DATE) << std::endl;

    LOG_INFO << "Done " << argv[0] << std::endl;

    return 0;
}
