#pragma once

#include <vector>

#include "types.h"


struct Theta {
    // document -> topic -> probability
    std::vector<
        std::vector<float_type>
    > matrix;

    Theta() {}

    Theta(
        size_type documents_count,
        size_type topics_count,
        float_type (*initializer)()
    );

    void normalize_into(Theta & other) const;
    void fill(float_type (*initializer)());

    // utility methods
    std::vector<float_type> & operator [] (id_type document_id);
    const std::vector<float_type> & operator [] (id_type document_id) const;
};
