#pragma once

#include <vector>

#include "types.h"


struct Theta {
    // document -> topic -> probability
    std::vector<
        std::vector<float_type>
    > matrix;

    Theta(
        size_type documents_count,
        size_type topics_count,
        float_type (*initializer)()
    );

    void normalize();
    void fill(float_type (*initializer)());
};

