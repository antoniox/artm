#include <fstream>

#include <boost/program_options.hpp>

#include "document_record.h"
#include "vocabulary.h"
#include "raw_record.h"
#include "utils.h"


namespace po = boost::program_options;


void set_options(po::options_description & description) {
    description.add_options()
        ("raw-corpus", po::value<std::string>(), "raw corpus filename")
        ("corpus", po::value<std::string>(), "processed corpus filename")
        ("vocabulary", po::value<std::string>(), "vocabulary filename");
}


int main(int argc, const char ** argv) {
    auto options = parse_options(&set_options, argc, argv);

    std::ifstream raw_corpus_stream(options["raw-corpus"].as<std::string>());
    std::ofstream corpus_stream(options["corpus"].as<std::string>());
    std::ofstream vocabulary_stream(options["vocabulary"].as<std::string>());

    RawRecord raw_record;
    Vocabulary vocabulary;

    // skip header
    skip_line(raw_corpus_stream);

    DocumentRecord::save_header(corpus_stream);

    while (raw_corpus_stream) {
        raw_record.load(raw_corpus_stream);

        auto tokenizer = raw_record.tokenize(vocabulary);

        for (auto & document_record : tokenizer) {
            document_record.save(corpus_stream);
        }
    }

    vocabulary.save(vocabulary_stream);

    return 0;
}
