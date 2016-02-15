#pragma once

#include <fstream>

#include "batch.h"
#include "phi.h"
#include "types.h"
#include "vocabulary.h"


class BatchProcessor {
public:
    BatchProcessor(
        size_type topics_count,
        size_type inner_iterations,
        const Vocabulary & vocabulary
    );

    void process(const Batch & batch);
    void save_model(std::ostream & output) const;

private:
    size_type topics_count_;
    size_type inner_iterations_;

    Phi phi_;
    Phi phi_counters_;
};
