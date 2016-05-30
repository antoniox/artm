#pragma once

#include <array>
#include <fstream>
#include <vector>

#include <boost/heap/priority_queue.hpp>

#include "vocabulary.h"
#include "phi.h"
#include "types.h"


struct TopTokens {
    typedef std::vector<
                std::array<
                    boost::heap::priority_queue<TokenProbability>,
                    MODALITY_COUNT
                >
            > TopTypeSlice;

    // type -> topic -> modality -> top tokens with probabilities
    std::array<TopTypeSlice, TYPE_COUNT> matrix;
    std::vector<size_type> token_counts_;

    TopTokens(
        size_type topics_count, 
        const std::vector<size_type> & token_counts
    );

    void fill_from(const Phi & phi);
    void print(const Vocabulary & vocabulary, std::ostream & output);
};
