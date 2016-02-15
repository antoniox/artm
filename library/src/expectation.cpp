#include "expectation.h"
#include "utils.h"


Expectation::Expectation(
    const std::vector<Document> & documents,
    size_type topics_count,
    float_type (*initializer)()
) {
    size_type documents_count = documents.size();

    matrix.reserve(documents_count);

    FOR(id_type, document_id, documents_count) {
        auto & document = documents[document_id]; 

        EdgeMapping edge_mapping;

        for (auto & edge_entry : document.edge_entries) {
            auto & edge = edge_entry.edge;
            auto & edge_slice = edge_mapping[edge];

            edge_slice.reserve(topics_count);

            FOR(id_type, topic_id, topics_count) {
                edge_slice.push_back((*initializer)());
            }
        }

        matrix.push_back(std::move(edge_mapping));
    }
}


void Expectation::normalize() {
    for (auto & document_slice : matrix) {
        for (auto & pair : document_slice) {
            auto & edge_slice = pair.second;

            float_type divisor = 0;

            for (auto & value : edge_slice) {
                divisor += value;
            }

            if (divisor > 0) {
                for (auto & value : edge_slice) {
                    value /= divisor;
                }
            }
        }
    }
}


void Expectation::fill(float_type (*initializer)()) {
    for (auto & document_slice : matrix) {
        for (auto & pair : document_slice) {
            auto & edge_slice = pair.second;

            for (auto & value : edge_slice) {
                value = (*initializer)();
            }
        }
    }
}
