#include "constants.h"
#include "phi.h"
#include "types.h"
#include "utils.h"


Phi::Phi(
    size_type topics_count, 
    const Vocabulary & vocabulary,
    float_type (*initializer)()
) {
    PARALLEL_FOR(id_type, type_id, TYPE_COUNT) {
        auto & type_slice = matrix[type_id];
        type_slice.reserve(topics_count);

        FOR(id_type, topic_id, topics_count) {
            type_slice.push_back(
                std::array<
                    std::vector<float_type>,
                    MODALITY_COUNT
                >()
            );

            auto & topic_slice = type_slice.back();

            FOR(id_type, modality_id, MODALITY_COUNT) {
                Modality modality = static_cast<Modality>(modality_id);
                size_type tokens_count = vocabulary.modality_size(modality);

                auto & modality_slice = topic_slice[modality];
                modality_slice.reserve(tokens_count);

                FOR(id_type, token_id, tokens_count) {
                    modality_slice.push_back((*initializer)());
                }
            }
        }
    }

    normalize();
}


void Phi::normalize() {
    for (auto & type_slice : matrix) {
        FOR(id_type, topic_id, type_slice.size()) {
            auto & topic_slice = type_slice[topic_id];
            for (auto & modality_slice : topic_slice) {
                float_type divisor = 0;

                for (auto & value : modality_slice) {
                    divisor += value; 
                }

                if (divisor > 0) {
                    for (auto & value : modality_slice) {
                        value /= divisor;
                    }
                }
            }
        }
    }
}


void Phi::fill(float_type (*initializer)()) {
    for (auto & type_slice : matrix) {
        PARALLEL_FOR(id_type, topic_id, type_slice.size()) {
            auto & topic_slice = type_slice[topic_id];
            for (auto & modality_slice : topic_slice) {
                for (auto & value : modality_slice) {
                    value = (*initializer)();
                }
            }
        }
    }
}


void Phi::save(std::ostream & output) const {
    // TODO: rewrite using phi_record
    output << 
        "type" << DELIMITER <<
        "topic_id" << DELIMITER <<
        "modality" << DELIMITER <<
        "token_id" << DELIMITER <<
        "value" << DELIMITER << std::endl;

    FOR(id_type, type_id, matrix.size()) {
        Type type = static_cast<Type>(type_id);
        auto & type_slice = matrix[type];

        FOR(id_type, topic_id, type_slice.size()) {
            auto & topic_slice = type_slice[topic_id];

            FOR(id_type, modality_id, topic_slice.size()) {
                Modality modality = static_cast<Modality>(modality_id);
                auto & modality_slice = topic_slice[modality];

                FOR(id_type, token_id, modality_slice.size()) {
                    auto & value = modality_slice[token_id];

                    output << 
                        type << DELIMITER <<
                        topic_id << DELIMITER <<
                        modality << DELIMITER <<
                        token_id << DELIMITER <<
                        value << DELIMITER << std::endl;
                }
            }
        }
    }
}
