#pragma once

#include <array>
#include <fstream>
#include <vector>

#include "types.h"
#include "vocabulary.h"


struct Phi {
    typedef std::vector<
                std::array<
                    std::vector<float_type>,
                    MODALITY_COUNT
                >
            > PhiTypeSlice;

    // type -> topic -> modality -> token -> probability
    std::array<PhiTypeSlice, TYPE_COUNT> matrix;

    Phi(
        size_type topics_count, 
        const std::vector<size_type> & token_counts,
        float_type (*initializer)()
    );

    void normalize_into(Phi & other) const;
    void fill(float_type (*initializer)());
    void increment(const Phi & other);

    void save(std::ostream & output) const;
    void load(std::istream & istream);

    // utility methods
    PhiTypeSlice & operator [] (const Type & type);
    const PhiTypeSlice & operator [] (const Type & type) const;
};
