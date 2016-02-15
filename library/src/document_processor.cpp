#include <vector>

#include "document_processor.h"
#include "theta.h"
#include "expectation.h"
#include "utils.h"


DocumentProcessor::DocumentProcessor(
    size_type topics_count,
    size_type inner_iterations,
    const Vocabulary & vocabulary
) : topics_count_(topics_count),
    inner_iterations_(inner_iterations),
    phi_(topics_count, vocabulary, &uniform_random),
    phi_counters_(phi_) {

    phi_counters_.fill(&zero);
}


void DocumentProcessor::process(const Document & document) {
    const size_type documents_count = 1;

    Theta theta(documents_count, topics_count_, &uniform_random);

    auto & document_theta = theta.matrix[0];

    Theta theta_counters = theta;
    theta_counters.fill(&zero);

    auto & document_theta_counters = theta_counters.matrix[0];

    Expectation expectation(
        {document}, topics_count_, &zero
    );

    auto & document_expectation = expectation.matrix[0];

    auto & type_phi = phi_.matrix[FIRST];
    auto & type_phi_counters = phi_counters_.matrix[FIRST];

    FOR(id_type, iteration, inner_iterations_) {
        // expectation
        PARALLEL_FOR(id_type, topic_id, topics_count_) {
            float_type value = document_theta[topic_id];
            auto & phi_slice = type_phi[topic_id];

            for (const auto & edge_entry : document.edge_entries) {
                auto & edge = edge_entry.edge;
                auto & edge_expectation = document_expectation[edge];

                for (const auto & token : edge.tokens) {
                    value *= phi_slice[token.modality][token.id];
                }

                edge_expectation[topic_id] += value;
            }
        }

        expectation.normalize();

        // maximization of theta
        for (const auto & edge_entry : document.edge_entries) {
            auto & edge = edge_entry.edge;
            auto & entries = edge_entry.entries;

            auto & edge_expectation = document_expectation[edge];

            FOR(id_type, topic_id, topics_count_) {
                document_theta_counters[topic_id] +=
                    entries * edge_expectation[topic_id];
            }
        }

        theta_counters.normalize();

        theta.matrix.swap(theta_counters.matrix);

        theta_counters.fill(&zero);
    }

    // maximization of phi
    // PARALLEL_FOR(id_type, topic_id, topics_count_) {
    //     auto & topic_phi_counters = type_phi_counters[topic_id];
    //
    //     for (const auto & edge_entry : document.edge_entries) {
    //         auto & edge = edge_entry.edge;
    //         auto & entries = edge_entry.entries;
    //
    //         auto & edge_expectation = document_expectation[edge];
    //         float_type value = entries * edge_expectation[topic_id];
    //
    //         for (const auto & token : edge.tokens) {
    //             topic_phi_counters[token.modality][token.id] += value;
    //         }
    //     }
    // }
    //
    // phi_counters_.normalize();
    //
    // phi_.matrix.swap(phi_counters_.matrix);
    // phi_counters_.fill(&zero);
}
