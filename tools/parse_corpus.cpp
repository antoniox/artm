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
        ("corpus-directory", po::value<std::string>(), "corpus directory path")
        ("vocabulary-directory", po::value<std::string>(), "vocabulary directory path")
        ("raw-corpus", po::value<std::string>(), "raw corpus filename")
        ("corpus", po::value<std::string>(), "processed corpus filename")
        ("vocabulary", po::value<std::string>(), "vocabulary filename");
}


int main(int argc, const char ** argv) {
    auto options = parse_options(&set_options, argc, argv);

    init_logging(argv[0], options);
    LOG_INFO << "Running " << argv[0] << "..." <<  std::endl;
    LOG_INFO << options;

    auto corpus_directory = options["corpus-directory"].as<std::string>();

    auto raw_corpus_filename = options["raw-corpus"].as<std::string>();
    raw_corpus_filename = corpus_directory + "/" + raw_corpus_filename;

    auto corpus_filename = options["corpus"].as<std::string>();
    corpus_filename = corpus_directory + "/" + corpus_filename;

    auto vocabulary_directory = options["vocabulary-directory"].as<std::string>();
    auto vocabulary_filename = options["vocabulary"].as<std::string>();
    vocabulary_filename = vocabulary_directory + "/" + vocabulary_filename;

    std::ifstream raw_corpus_stream(raw_corpus_filename);
    std::ofstream corpus_stream(corpus_filename);
    std::ofstream vocabulary_stream(vocabulary_filename);

    RawRecord raw_record;
    Vocabulary vocabulary;

    // skip header
    skip_line(raw_corpus_stream);

    while (raw_record.load(raw_corpus_stream)) {
        auto tokenizer = raw_record.tokenize(vocabulary);

        for (auto & document_record : tokenizer) {
            if (document_record.is_valid_) {
                document_record.save(corpus_stream);
            }
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
