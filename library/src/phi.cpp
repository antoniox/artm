#include "constants.h"
#include "phi.h"
#include "phi_record.h"
#include "types.h"
#include "utils.h"


Phi::Phi(
    size_type topics_count, 
    const std::vector<size_type> & token_counts,
    float_type (*initializer)()
) {
    FOR(id_type, type_id, TYPE_COUNT) {
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
                size_type tokens_count = token_counts[modality_id];

                auto & modality_slice = topic_slice[modality];
                modality_slice.reserve(tokens_count);

                FOR(id_type, token_id, tokens_count) {
                    modality_slice.push_back((*initializer)());
                }
            }
        }
    }
}


void Phi::normalize_into(Phi & other) const {
    FOR(id_type, type_id, matrix.size()) {
        const auto & type_slice = matrix[type_id];
        auto & other_type_slice = other.matrix[type_id];

        PARALLEL_FOR(id_type, topic_id, type_slice.size()) {
            const auto & topic_slice = type_slice[topic_id];
            auto & other_topic_slice = other_type_slice[topic_id];

            FOR(id_type, modality_id, topic_slice.size()) {
                const auto & modality_slice = topic_slice[modality_id];
                auto & other_modality_slice = other_topic_slice[modality_id];

                float_type divisor = 0;

                for (const auto & value : modality_slice) {
                    divisor += value; 
                }

                FOR(id_type, token_id, modality_slice.size()) {
                    auto & value = other_modality_slice[token_id];
                    value = divisor == 0 ? 0 : modality_slice[token_id] / divisor;
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
    PhiRecord record;

    FOR(id_type, type_id, matrix.size()) {
        Type type = static_cast<Type>(type_id);
        auto & type_slice = matrix[type];

        record.type = type;

        FOR(id_type, topic_id, type_slice.size()) {
            auto & topic_slice = type_slice[topic_id];
            record.topic_id = topic_id;

            FOR(id_type, modality_id, topic_slice.size()) {
                Modality modality = static_cast<Modality>(modality_id);
                auto & modality_slice = topic_slice[modality];
                record.modality = modality;

                FOR(id_type, token_id, modality_slice.size()) {
                    record.token_id = token_id;
                    record.value = modality_slice[token_id];

                    record.save(output);
                }
            }
        }
    }
}


void Phi::load(std::istream & input) {
    PhiRecord record;

    while (record.load(input)) {
        matrix[record.type][record.topic_id][record.modality][record.token_id] =
            record.value;
    }
}


Phi::PhiTypeSlice & Phi::operator [] (const Type & type) {
    return matrix[type];
}


const Phi::PhiTypeSlice & Phi::operator [] (const Type & type) const {
    return matrix[type];
}
