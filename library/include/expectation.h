#pragma once

#include <vector>
#include <unordered_map>

#include "document.h"
#include "types.h"


struct Expectation {
    typedef
        std::unordered_map<
            Edge, std::vector<float_type>
        > EdgeMapping;

    // document -> edge -> topic -> probability
    std::vector<EdgeMapping> matrix;

    Expectation(
        const std::vector<Document> & documents,
        size_type topics_count,
        float_type (*initializer)()
    );

    void normalize();
    void fill(float_type (*initializer)());

    // utility methods
    EdgeMapping & operator [] (id_type document_id);
    const EdgeMapping & operator [] (id_type document_id) const;
};
