#include <array>
#include <fstream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "document.h"
#include "modality.h"
#include "typedefs.h"
#include "utils.h"
#include "vocabulary.h"


float_type uniform_random() {
    static std::random_device random_device;
    static std::mt19937 random_generator(random_device());
    static std::uniform_real_distribution<float_type> distribution(0, 1);

    return distribution(random_generator);
}

typedef
    std::array<
        std::vector<
            std::array<
                std::vector<float_type>,
                MODALITY_COUNT
            >
        >,
        TYPE_COUNT
    > PhiMatrix;


void init_phi_matrix(
    size_type topics_count,
    const Vocabulary & vocabulary,
    float_type (*init_element)(),
    PhiMatrix & matrix
) {
    for (id_type type_id = 0; type_id < TYPE_COUNT; ++type_id) {
        auto & type_slice = matrix[type_id];
        type_slice.reserve(topics_count);

        for (id_type topic_id = 0; topic_id < topics_count; ++topic_id) {
            type_slice.push_back(
                std::array<
                    std::vector<float_type>,
                    MODALITY_COUNT
                >()
            );

            auto & topic_slice = type_slice.back();

            for (id_type modality_id = 0; modality_id < MODALITY_COUNT; ++modality_id) {
                Modality modality = static_cast<Modality>(modality_id);
                size_type token_count = vocabulary.modality_size(modality);

                auto & modality_slice = topic_slice[modality];
                modality_slice.reserve(token_count);

                for (id_type token_id = 0; token_id < token_count; ++token_id) {
                    modality_slice.push_back((*init_element)());
                }
            }
        }
    }
}


void clear_phi_matrix(PhiMatrix & matrix, float_type (*clear_element)()) {
    for (auto & type_slice : matrix) {
        for (auto & topic_slice : type_slice) {
            for (auto & modality_slice : topic_slice) {
                for (
                    auto iterator = modality_slice.begin();
                    iterator != modality_slice.end();
                    ++iterator
                ) {
                    *iterator = (*clear_element)();
                }
            }
        }
    }
}


struct Phi {
    // type -> topic -> modality -> token -> probability
    PhiMatrix matrix;

    Phi(
        size_type topics_count, 
        const Vocabulary & vocabulary
    ) {
        init_phi_matrix(topics_count, vocabulary, &uniform_random, matrix);
        normalize();
    }

    void normalize() {
        for (auto & type_slice : matrix) {
            for (auto & topic_slice : type_slice) {
                for (auto & modality_slice : topic_slice) {
                    float_type divisor = 0;

                    for (auto & value : modality_slice) {
                        divisor += value; 
                    }

                    for (
                        auto iterator = modality_slice.begin();
                        iterator != modality_slice.end();
                        ++iterator
                    ) {
                        *iterator /= divisor;
                    }
                }
            }
        }
    }
};


struct Theta {
    // topic -> probability
    std::vector<float_type> matrix;

    Theta(size_type topics_count) { 
        matrix.reserve(topics_count);

        for (size_type topic_id = 0; topic_id < topics_count; ++topic_id) {
            matrix.push_back(uniform_random());
        }

        normalize();
    }

    void normalize() {
        float_type divisor = 0;

        for (auto & value : matrix) {
            divisor += value;
        }

        for (
            auto iterator = matrix.begin();
            iterator != matrix.end();
            ++iterator
        ) {
            *iterator /= divisor;
        }
    }
};


void set_options(po::options_description & description) {
    description.add_options()
        ("config",
            po::value<std::string>()->default_value("process.cfg"),
            "config filename")
        ("corpus", po::value<std::string>(), "corpus filename")
        ("vocabulary", po::value<std::string>(), "vocabulary filename")
        ("topics_count", po::value<size_type>(), "number of topics used for processing")
        ("inner_iterations", po::value<size_type>(), "number of inner iterations");
}


void process_document(
    size_type topics_count, size_type inner_iterations,
    Document & document,
    Phi & phi, PhiMatrix & phi_counters
) {
    size_type edges_count = document.edge_entries.size();

    Theta theta(topics_count);

    // simplified version for a single document
    std::vector<
        std::vector<float_type>
    > expectations(
        edges_count,
        std::vector<float_type> (topics_count, 0)
    );

    std::vector<float_type> expectation_sums(edges_count, 0);

    std::vector<float_type> theta_counters(topics_count, 0);
    float_type theta_counter_sums = 0;

    std::array<
        std::vector<
            std::array<
                float_type,
                MODALITY_COUNT
            >
        >,
        TYPE_COUNT
    > phi_counter_sums;

    for (auto & type_slice : phi_counter_sums) {
        type_slice.reserve(topics_count);

        for (id_type topic_id = 0; topic_id < topics_count; ++topic_id) {
            type_slice.push_back(
                std::array<
                    float_type,
                    MODALITY_COUNT
                > ()
            );
        }
    }

    for (size_type iteration = 0; iteration < inner_iterations; ++iteration) {
        // expectation
        for (const auto & edge_entry : document.edge_entries) {
            auto & edge = edge_entry.edge;

            auto & edge_expectations = expectations[edge.id];
            auto & edge_expectation_sums = expectation_sums[edge.id];

            for (id_type topic_id = 0; topic_id < topics_count; ++topic_id) {
                float_type value = theta.matrix[topic_id];
                auto & phi_slice = phi.matrix[FIRST][topic_id];

                for (const auto & token : edge.tokens) {
                    value *= phi_slice[token.modality][token.id];
                }

                edge_expectations[topic_id] += value;
                edge_expectation_sums += value;
            }
        }

        // normalization of expectation
        for (const auto & edge_entry : document.edge_entries) {
            auto & edge = edge_entry.edge;

            auto & edge_expectations = expectations[edge.id];
            auto & edge_expectation_sums = expectation_sums[edge.id];

            for (id_type topic_id = 0; topic_id < topics_count; ++topic_id) {
                edge_expectations[topic_id] /= edge_expectation_sums;
            }
        }

        // maximization of theta
        for (const auto & edge_entry : document.edge_entries) {
            auto & edge = edge_entry.edge;
            auto & entries = edge_entry.entries;

            for (id_type topic_id = 0; topic_id < topics_count; ++topic_id) {
                float_type value = entries * expectations[edge.id][topic_id];
                theta_counters[topic_id] += value;
                theta_counter_sums += value;
            }
        }

        // normalization of theta counters
        for (id_type topic_id = 0; topic_id < topics_count; ++topic_id) {
            theta_counters[topic_id] /= theta_counter_sums;
        }

        theta.matrix.swap(theta_counters);
    }

    // maximization of phi
    for (const auto & edge_entry : document.edge_entries) {
        auto & edge = edge_entry.edge;
        auto & entries = edge_entry.entries;

        auto & expectations_slice = expectations[edge.id];

        auto & phi_counters_type = phi_counters[edge.type];
        auto & phi_counter_type_sums = phi_counter_sums[edge.type];

        for (id_type topic_id = 0; topic_id < topics_count; ++topic_id) {
            float_type value = entries * expectations_slice[topic_id];

            auto & phi_counters_slice = phi_counters_type[topic_id];
            auto & phi_counter_sums_slice = phi_counter_type_sums[topic_id];

            for (const auto & token : edge.tokens) {
                phi_counters_slice[token.modality][token.id] += value;
                phi_counter_sums_slice[token.modality] += value;
            }
        }
    }

    // normalization of phi counters
    for (id_type type_id = 0; type_id < TYPE_COUNT; ++type_id) {
        Type type = static_cast<Type>(type_id);

        auto & phi_counters_type = phi_counters[type];
        auto & phi_counter_type_sums = phi_counter_sums[type];

        for (id_type topic_id = 0; topic_id < topics_count; ++topic_id) {
            auto & phi_counters_topic = phi_counters_type[topic_id];
            auto & phi_counter_topic_sums = phi_counter_type_sums[topic_id];

            for (id_type modality_id = 0; modality_id < MODALITY_COUNT; ++modality_id) {
                Modality modality = static_cast<Modality>(modality_id);

                auto & phi_counters_slice = phi_counters_topic[modality];
                auto & phi_counter_sums_slice = phi_counter_topic_sums[modality];

                for (
                    auto iterator = phi_counters_slice.begin();
                    iterator != phi_counters_slice.end();
                    ++iterator
                ) {
                    *iterator /= phi_counter_sums_slice;
                }
            }
        }
    }

    phi.matrix.swap(phi_counters);
    clear_phi_matrix(phi_counters, [] () -> float_type { return 0; });
}


int main(int argc, const char ** argv) {
    init_logging(argv[0]);

    auto options = parse_options(&set_options, argc, argv);
    LOG_INFO << options;

    std::ifstream vocabulary_stream(options["vocabulary"].as<std::string>());

    Vocabulary vocabulary;
    vocabulary.load(vocabulary_stream);

    size_type topics_count = options["topics_count"].as<size_type>();

    Phi phi(topics_count, vocabulary);

    PhiMatrix phi_counters;
    init_phi_matrix(
        topics_count, vocabulary, [] () -> float_type { return 0; },
        phi_counters
    );

    std::ifstream corpus_stream(options["corpus"].as<std::string>());

    // skip header
    skip_line(corpus_stream);

    DocumentParser document_parser(corpus_stream);

    size_type inner_iterations = options["inner_iterations"].as<size_type>();

    Document document;

    id_type document_id = 0;

    while (document_parser.has_more()) {
        if (document_id >= 1001) {
            break;
        }

        document.edge_entries.clear();
        document_parser.parse(document);

        process_document(
            topics_count, inner_iterations,
            document,
            phi, phi_counters
        );

        LOG_INFO_EVERY_N(100, "documents");
        ++document_id;
    }

    return 0;


}
