#include <array>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>

#include "document_record.h"
#include "types.h"
#include "utils.h"
#include "vocabulary_record.h"


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
        ("config",
            po::value<std::string>()->default_value("shrink_corpus.cfg"),
            "config filename")
        ("corpus", po::value<std::string>(), "processed corpus filename")
        ("vocabulary", po::value<std::string>(), "vocabulary filename")
        ("suffix", po::value<std::string>(), "suffix of output data");

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
        // Threshold {
        //     options["min-date-threshold"].as<size_type>(),
        //     options["max-date-threshold"].as<size_type>(),
        // }
    };
}


void fill_frequencies(std::istream & input, Frequencies & frequencies) {
    LOG_INFO << "Filling token frequencies..." << std::endl;

    DocumentRecord record;

    auto & word_frequencies = frequencies[WORD];
    auto & nick_frequencies = frequencies[NICK];
    auto & date_frequencies = frequencies[DATE];

    while (record.load(input)) {
        id_type document_id = record.document_id;
        id_type word_id = record.word_id;
        id_type nick_id = record.nick_id;
        id_type date_id = record.date_id;

        word_frequencies[word_id].insert(document_id);
        nick_frequencies[nick_id].insert(document_id);
        date_frequencies[date_id].insert(document_id);

        LOG_INFO_EVERY_N(1000000, "rows");
    }

    LOG_INFO << "Done filling token frequencies" << std::endl;
    LOG_INFO << "Token statistics: \n" <<
        "\twords: " << word_frequencies.size() << std::endl <<
        "\tnicks: " << nick_frequencies.size() << std::endl <<
        "\tdates: " << date_frequencies.size() << std::endl;
}


void fill_remapping_vocabulary(
    const Frequencies & frequencies,
    const Thresholds & thresholds,
    RemappingVocabulary & remapping_vocabulary
) {
    LOG_INFO << "Filling remapping vocabulary..." << std::endl;

    std::array<id_type, MODALITY_COUNT> candidate_ids({0, 0});// , 0});

    FOR(id_type, modality_id, MODALITY_COUNT) {
        Modality modality = static_cast<Modality>(modality_id);
        LOG_INFO << "Processing tokens for modality: " << modality << std::endl;

        auto & threshold = thresholds[modality];
        auto & minimum = threshold.minimum;
        auto & maximum = threshold.maximum;

        auto & modality_remapping = remapping_vocabulary[modality];
        auto & token_frequencies = frequencies[modality];

        for (auto & token_documents : token_frequencies) {
            auto & token = token_documents.first;
            auto & documents = token_documents.second;

            auto documents_count = documents.size();

            if (minimum <= documents_count && documents_count <= maximum) {
                modality_remapping[token] = candidate_ids[modality];
                candidate_ids[modality] += 1;
            }

            LOG_INFO_EVERY_N(10000, "tokens");
        }
    }

    LOG_INFO << "Done filling remapping vocabulary" << std::endl;
    LOG_INFO << "Token statistics: \n" << 
        "\twords: " << remapping_vocabulary[WORD].size() << std::endl <<
        "\tnicks: " << remapping_vocabulary[NICK].size() << std::endl <<
        "\tdates: " << remapping_vocabulary[DATE].size() << std::endl;
}


void remap_vocabulary(
    const RemappingVocabulary & remapping_vocabulary,
    std::istream & input,
    std::ostream & output
) {
    LOG_INFO << "Remapping vocabulary..." << std::endl;

    VocabularyRecord record;

    while (record.load(input)) {
        auto & modality = record.modality;
        auto & token_id = record.token_id;

        auto & modality_remapping = remapping_vocabulary[modality];
        auto iterator = modality_remapping.find(token_id);

        if (iterator != modality_remapping.end()) {
            record.token_id = iterator->second;
            record.save(output);
        }

        LOG_INFO_EVERY_N(10000, "rows");
    }

    LOG_INFO << "Done remapping vocabulary" << std::endl;
}


void remap_corpus(
    const RemappingVocabulary & remapping_vocabulary,
    std::istream & input,
    std::ostream & output
) {
    LOG_INFO << "Remapping corpus..." << std::endl;

    auto & word_remapping = remapping_vocabulary[WORD];
    auto & nick_remapping = remapping_vocabulary[NICK];
    auto & date_remapping = remapping_vocabulary[DATE];

    DocumentRecord record;

    while (record.load(input)) {
        LOG_INFO_EVERY_N(1000000, "rows");

        id_type word_id = record.word_id;
        id_type nick_id = record.nick_id;
        id_type date_id = record.date_id;

        auto word_iterator = word_remapping.find(word_id);

        if (word_iterator == word_remapping.end()) {
            continue;
        }

        auto nick_iterator = nick_remapping.find(nick_id);

        if (nick_iterator == nick_remapping.end()) {
            continue;
        }

        auto date_iterator = date_remapping.find(date_id);

        if (date_iterator == date_remapping.end()) {
            continue;
        }

        record.word_id = word_iterator->second;
        record.nick_id = nick_iterator->second;
        record.date_id = date_iterator->second;

        record.save(output);
    }

    LOG_INFO << "Done remapping corpus" << std::endl;
}


int main(int argc, const char ** argv) {
    auto options = parse_options(&set_options, argc, argv);

    init_logging(argv[0], options);
    LOG_INFO << "Running " << argv[0] << "..." <<  std::endl;
    LOG_INFO << options;

    auto thresholds = parse_thresholds(options);

    auto corpus_filename = options["corpus"].as<std::string>();
    auto vocabulary_filename = options["vocabulary"].as<std::string>();
    auto suffix = options["suffix"].as<std::string>();

    auto output_corpus_filename = corpus_filename + suffix;
    auto output_vocabulary_filename = vocabulary_filename + suffix;

    std::ifstream input_vocabulary_stream(vocabulary_filename);
    std::ofstream output_vocabulary_stream(output_vocabulary_filename);

    std::ifstream input_corpus_stream(corpus_filename);
    std::ofstream output_corpus_stream(output_corpus_filename);

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

    LOG_INFO << "Done " << argv[0] << std::endl;

    return 0;
}
