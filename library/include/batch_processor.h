#pragma once

#include <fstream>
#include <vector>

#include "batch.h"
#include "expectation.h"
#include "phi.h"
#include "theta.h"
#include "types.h"
#include "vocabulary.h"


class BatchProcessor {
public:
    BatchProcessor(
        size_type topics_count,
        size_type inner_iterations,
        const std::vector<size_type> & token_counts
    );

    void process(const Batch & batch);
    void save_model(std::ostream & output) const;

private:
    size_type topics_count_;
    size_type inner_iterations_;

    Phi phi_;
    Phi phi_counters_;

    void calculate_expectation(
        const std::vector<Document> & documents,
        const Theta & theta,
        Expectation & expectation
    ) const;

    void maximize_theta(
        const std::vector<Document> & documents,
        const Expectation & expectation,
        Theta & theta,
        Theta & theta_counters
    ) const;

    void maximize_phi(
        const std::vector<Document> & documents,
        const Expectation & expectation
    );
};
