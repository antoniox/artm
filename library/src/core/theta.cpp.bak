#include "theta.h"
#include "utils.h"


Theta::Theta(
    size_type documents_count,
    size_type topics_count,
    float_type (*initializer)()
) {
    matrix.reserve(documents_count);

    FOR(id_type, document_id, documents_count) {
        std::vector<float_type> document_slice;
        document_slice.reserve(topics_count);

        FOR(id_type, topic_id, topics_count) {
            document_slice.push_back((*initializer)());
        }

        matrix.push_back(document_slice);
    }
}


void Theta::normalize_into(Theta & other) const {
    PARALLEL_FOR(id_type, document_id, matrix.size()) {
        const auto & document_slice = matrix[document_id];
        auto & other_document_slice = other[document_id];

        float_type divisor = 0;

        for (const auto & value : document_slice) {
            divisor += value;
        }

        FOR(id_type, topic_id, document_slice.size()) {
            if (divisor == 0) {
                other_document_slice[topic_id] = 0;
            } else {
                other_document_slice[topic_id] = document_slice[topic_id] / divisor;
            }
        }
    }
}


void Theta::fill(float_type (*initializer)()) {
    for (auto & document_slice : matrix) {
        for (auto & value : document_slice) {
            value = (*initializer)();
        }
    }
}


std::vector<float_type> & Theta::operator [] (id_type document_id) {
    return matrix[document_id];
}


const std::vector<float_type> & Theta::operator [] (id_type document_id) const {
    return matrix[document_id];
}
