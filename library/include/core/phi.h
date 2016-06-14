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
    size_type topics_count_;
    std::vector<size_type> token_counts_;

    Phi(const std::string & filename) {
        load(filename);
    };

    Phi(
        size_type topics_count, 
        const std::vector<size_type> & token_counts,
        float_type (*initializer)()
    ) :
        topics_count_(topics_count),
        token_counts_(token_counts) {

        create(topics_count, token_counts, initializer);
    }

    void normalize_into(Phi & other) const;
    void fill(float_type (*initializer)());

    void create(
        size_type topics_count, 
        const std::vector<size_type> & token_counts,
        float_type (*initializer)()
    );

    void save(std::ostream & output) const;
    void save(const std::string & filename) const {
        std::ofstream output(filename);
        save(output);
    }

    void load(std::istream & input);
    void load(const std::string & filename) {
        std::ifstream input(filename);
        load(input);
    }

    // utility methods
    PhiTypeSlice & operator [] (const Type & type);
    const PhiTypeSlice & operator [] (const Type & type) const;
};
