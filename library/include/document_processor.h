#pragma once

#include "document.h"
#include "phi.h"
#include "types.h"
#include "vocabulary.h"


class DocumentProcessor {
public:
    DocumentProcessor(
        size_type topics_count,
        size_type inner_iterations,
        const Vocabulary & vocabulary
    );

    void process(const Document & document);

private:
    size_type topics_count_;
    size_type inner_iterations_;

    Phi phi_;
    Phi phi_counters_;
};
