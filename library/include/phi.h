#pragma once

#include <array>
#include <fstream>
#include <vector>

#include "types.h"
#include "vocabulary.h"


struct Phi {
    // type -> topic -> modality -> token -> probability
    std::array<
        std::vector<
            std::array<
                std::vector<float_type>,
                MODALITY_COUNT
            >
        >,
        TYPE_COUNT
    > matrix;

    Phi(
        size_type topics_count, 
        const Vocabulary & vocabulary,
        float_type (*initializer)()
    );

    void normalize();
    void fill(float_type (*initializer)());
    void save(std::ostream & output) const;
};
