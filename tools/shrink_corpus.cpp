#include <array>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>

#include <boost/program_options.hpp>

#include "document_record.h"
#include "modality.h"
#include "typedefs.h"
#include "utils.h"
#include "vocabulary_record.h"


namespace po = boost::program_options;


typedef
    std::array<
        std::unordered_map<id_type, std::unordered_set<id_type>>,
        MODALITY_COUNT
    > Frequencies;

typedef
    std::array<
        std::unordered_map<id_type, id_type>,
        MODALITY_COUNT
    > RemappingVocabulary;


struct Threshold {
    size_type minimum;
    size_type maximum;
};

typedef
    std::array<
        Threshold,
        MODALITY_COUNT
    > Thresholds;


void add_threshold_options(
    const std::string & name,
    po::options_description & description 
) {
    std::string min_label = "min-" + name + "-threshold";
    std::string min_text = "minimum number of documents per " + name;

    std::string max_label = "max-" + name + "-threshold";
    std::string max_text = "maximum number of documents per " + name;

    description.add_options()
        (
            min_label.c_str(),
            po::value<size_type>()->default_value(
                std::numeric_limits<size_type>::min()
            ),
            min_text.c_str()
        )
        (
            max_label.c_str(),
            po::value<size_type>()->default_value(
                std::numeric_limits<size_type>::max()
            ),
            max_text.c_str()
        );
}


void set_options(po::options_description & description) {
    description.add_options()
        ("input-vocabulary", po::value<std::string>(), "input vocabulary filename")
        ("output-vocabulary", po::value<std::string>(), "output vocabulary filename")
        ("input-corpus", po::value<std::string>(), "input corpus filename")
        ("output-corpus", po::value<std::string>(), "output corpus filename");

    add_threshold_options("word", description);
    add_threshold_options("nick", description);
    add_threshold_options("date", description);
}


Thresholds parse_thresholds(const po::variables_map & options) {
    return {
        Threshold {
            options["min-word-threshold"].as<size_type>(),
            options["max-word-threshold"].as<size_type>(),
        },
        Threshold {
            options["min-nick-threshold"].as<size_type>(),
            options["max-nick-threshold"].as<size_type>(),
        },
        Threshold {
            options["min-date-threshold"].as<size_type>(),
            options["max-date-threshold"].as<size_type>(),
        }
    };
}


void fill_frequencies(std::istream & input, Frequencies & frequencies) {
    DocumentRecord record;

    // skip header
    skip_line(input);

    while (input) {
        record.load(input);

        id_type document_id = record.document_id;
        id_type word_id = record.word_id;
        id_type nick_id = record.nick_id;
        id_type date_id = record.date_id;

        frequencies[WORD][word_id].insert(document_id);
        frequencies[NICK][nick_id].insert(document_id);
        frequencies[DATE][date_id].insert(document_id);
    }
}


void fill_remapping_vocabulary(
    const Frequencies & frequencies,
    const Thresholds & thresholds,
    RemappingVocabulary & remapping_vocabulary
) {
    std::array<id_type, MODALITY_COUNT> candidate_ids({0, 0, 0});

    Modality modality = static_cast<Modality>(0);

    for (auto & token_frequencies : frequencies) {
        auto & threshold = thresholds[modality];
        auto & modality_remapping = remapping_vocabulary[modality];

        for (auto & token_documents : token_frequencies) {
            auto & token = token_documents.first;
            auto & documents = token_documents.second;

            auto documents_count = documents.size();

            if (
                threshold.minimum <= documents_count &&
                documents_count <= threshold.maximum
            ) {
                modality_remapping[token] = candidate_ids[modality];
                candidate_ids[modality] += 1;
            }
        }

        modality = static_cast<Modality>(modality + 1);
    }

    // XXX: log progress properly
    std::cerr <<
        "words\tnicks\tdates" << std::endl <<
        remapping_vocabulary[WORD].size() << '\t' <<
        remapping_vocabulary[NICK].size() << '\t' <<
        remapping_vocabulary[DATE].size() << std::endl;
}


void remap_vocabulary(
    const RemappingVocabulary & remapping_vocabulary,
    std::istream & input,
    std::ostream & output
) {
    VocabularyRecord record;

    // skip header
    skip_line(input);

    record.save_header(output);

    while (input) {
        record.load(input);

        auto & modality = record.modality;
        auto & token_id = record.token_id;

        auto & modality_remapping = remapping_vocabulary[modality];
        auto iterator = modality_remapping.find(token_id);

        if (iterator != modality_remapping.end()) {
            record.token_id = iterator->second;
            record.save(output);
        }
    }
}


void remap_corpus(
    const RemappingVocabulary & remapping_vocabulary,
    std::istream & input,
    std::ostream & output
) {
    DocumentRecord record;

    // skip header
    skip_line(input);

    record.save_header(output);

    while (input) {
        record.load(input);

        id_type document_id = record.document_id;
        id_type word_id = record.word_id;
        id_type nick_id = record.nick_id;
        id_type date_id = record.date_id;

        auto & word_remapping = remapping_vocabulary[WORD];
        auto & nick_remapping = remapping_vocabulary[NICK];
        auto & date_remapping = remapping_vocabulary[DATE];

        auto word_iterator = word_remapping.find(word_id);
        auto nick_iterator = nick_remapping.find(nick_id);
        auto date_iterator = date_remapping.find(date_id);

        if (
            word_iterator != word_remapping.end() &&
            nick_iterator != nick_remapping.end() &&
            date_iterator != date_remapping.end()
        ) {
            record.word_id = word_iterator->second;
            record.nick_id = nick_iterator->second;
            record.date_id = date_iterator->second;

            record.save(output);
        }
    }
}


int main(int argc, const char ** argv) {
    auto options = parse_options(&set_options, argc, argv);
    auto thresholds = parse_thresholds(options);

    std::ifstream input_vocabulary_stream(options["input-vocabulary"].as<std::string>());
    std::ofstream output_vocabulary_stream(options["output-vocabulary"].as<std::string>());

    std::ifstream input_corpus_stream(options["input-corpus"].as<std::string>());
    std::ofstream output_corpus_stream(options["output-corpus"].as<std::string>());

    Frequencies frequencies;
    fill_frequencies(input_corpus_stream, frequencies);

    RemappingVocabulary remapping_vocabulary;

    fill_remapping_vocabulary(
        frequencies, thresholds, remapping_vocabulary
    );

    remap_vocabulary(
        remapping_vocabulary, input_vocabulary_stream, output_vocabulary_stream
    );

    // prepare stream for second pass
    input_corpus_stream.clear();
    input_corpus_stream.seekg(0);

    remap_corpus(
        remapping_vocabulary, input_corpus_stream, output_corpus_stream
    );

    return 0;
}
