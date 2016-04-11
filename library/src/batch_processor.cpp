#include <vector>

#include "batch_processor.h"
#include "theta.h"
#include "expectation.h"
#include "utils.h"


BatchProcessor::BatchProcessor(
    size_type topics_count,
    size_type inner_iterations,
    const Vocabulary & vocabulary
) : topics_count_(topics_count),
    inner_iterations_(inner_iterations),
    phi_(topics_count, vocabulary, &uniform_random),
    phi_counters_(phi_) {
}


void BatchProcessor::calculate_expectation(
    const std::vector<Document> & documents,
    const Theta & theta,
    Expectation & expectation
) const {
    auto & type_phi = phi_[FIRST];
    size_type documents_count = documents.size();

    PARALLEL_FOR(id_type, document_id, documents_count) {
        auto & document = documents[document_id];
        const auto & document_theta = theta[document_id];
        auto & document_expectation = expectation[document_id];

        FOR(id_type, topic_id, topics_count_) {
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
    }

    expectation.normalize();
}


void BatchProcessor::maximize_theta(
    const std::vector<Document> & documents,
    const Expectation & expectation,
    Theta & theta,
    Theta & theta_counters
) const {
    size_type documents_count = documents.size();

    PARALLEL_FOR(id_type, document_id, documents_count) {
        auto & document = documents[document_id];
        auto & document_expectation = expectation[document_id];
        auto & document_theta_counters = theta_counters[document_id];

        for (const auto & edge_entry : document.edge_entries) {
            auto & edge = edge_entry.edge;
            auto & entries = edge_entry.entries;

            auto & edge_expectation = safe_get(document_expectation, edge);

            FOR(id_type, topic_id, topics_count_) {
                document_theta_counters[topic_id] +=
                    entries * edge_expectation[topic_id];
            }
        }
    }

    theta_counters.normalize();
    theta.swap(theta_counters);
    theta_counters.fill(&zero);
}


void BatchProcessor::maximize_phi(
    const std::vector<Document> & documents,
    const Expectation & expectation
) {
    auto & type_phi_counters = phi_counters_[FIRST];
    size_type documents_count = documents.size();

    PARALLEL_FOR(id_type, document_id, documents_count) {
        auto & document = documents[document_id];
        auto & document_expectation = expectation[document_id];

        FOR(id_type, topic_id, topics_count_) {
            auto & topic_phi_counters = type_phi_counters[topic_id];

            for (const auto & edge_entry : document.edge_entries) {
                auto & edge = edge_entry.edge;
                auto & entries = edge_entry.entries;

                auto & edge_expectation = safe_get(document_expectation, edge);
                float_type value = entries * edge_expectation[topic_id];

                for (const auto & token : edge.tokens) {
                    topic_phi_counters[token.modality][token.id] += value;
                }
            }
        }
    }

    phi_counters_.normalize();
    phi_.swap(phi_counters_);
    phi_counters_.fill(&zero);
}


void BatchProcessor::process(const Batch & batch) {
    auto & documents = batch.documents;
    size_type documents_count = documents.size();

    Theta theta(documents_count, topics_count_, &uniform_random);

    Theta theta_counters = theta;
    theta_counters.fill(&zero);

    Expectation expectation(documents, topics_count_, &zero);

    FOR(id_type, iteration, inner_iterations_) {
        calculate_expectation(documents, theta, expectation);
        maximize_theta(documents, expectation, theta, theta_counters);
    }

    maximize_phi(documents, expectation);
}


void BatchProcessor::save_model(std::ostream & output) const {
    phi_.save(output);
}
