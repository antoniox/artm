#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/corpus.h"
#include "corpora/livejournal.h"
#include "utils/misc.h"


void set_options(po::options_description & description) {
    description.add_options()
        ("config",
            po::value<std::string>()->default_value("parse_corpus.cfg"),
            "config filename")
        ("raw-corpus", po::value<std::string>(), "raw corpus filename")
        ("corpus", po::value<std::string>(), "corpus directory path")
        ("vocabulary", po::value<std::string>(), "vocabulary directory path")
        ("format", po::value<std::string>(), "raw corpus format")
        ("modalities", po::value<std::vector<std::string>>()->multitoken(), "modalities names")
        ("types", po::value<std::vector<std::string>>()->multitoken(), "allowed combinations of modalities");
}


int main(int argc, const char ** argv) {
    auto options = parse_options(&set_options, argc, argv);

    auto raw_corpus_filename = options["raw-corpus"].as<std::string>();
    auto corpus_filename = options["corpus"].as<std::string>();
    auto vocabulary_filename = options["vocabulary"].as<std::string>();
    auto format = options["format"].as<std::string>();
    auto modalities = options["modalities"].as<std::vector<std::string>>();
    auto types = options["types"].as<std::vector<std::string>>();

    // init_logging(argv[0], options);
    // LOG_INFO << "Running " << argv[0] << "..." <<  std::endl;
    // LOG_INFO << options;

    std::unordered_map<id_type, std::string> modality_names;
    FOR(modality_id, modalities.size()) {
        modality_names[modality_id] = modalities[modality_id];
    }

    Corpus corpus;

    if (format == "livejournal") {
        corpus = new LiveJournalCorpusImpl(raw_corpus_filename);
    }

    auto corpus_iterator_ptr = corpus.iterator();
    CorpusIterator & corpus_iterator = *corpus_iterator_ptr;

    while (corpus_iterator.has_more()) {
        auto document = *corpus_iterator;
        ++corpus_iterator;

        // LOG_INFO_EVERY_N(10000, "documents");
    }

    auto vocabulary = corpus.vocabulary();
    vocabulary.set_modality_names(modality_names);
    vocabulary.save(vocabulary_filename);

    // LOG_INFO << "Token statistics: \n" << 
    //     "\twords: " << vocabulary.modality_size(WORD) << std::endl <<
    //     "\tnicks: " << vocabulary.modality_size(NICK) << std::endl <<
    //     "\tdates: " << vocabulary.modality_size(DATE) << std::endl;
    //
    // LOG_INFO << "Done " << argv[0] << std::endl;

    return 0;
}
