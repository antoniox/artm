#pragma once

#include <array>
#include <fstream>
#include <string>
#include <unordered_map>

#include "modality.h"


class Vocabulary {
public:
    void load(std::istream & input);
    void save(std::ostream & output) const;

    size_t get_id(const Modality & modality, const std::string & token);

private:
    std::array<
        std::unordered_map<std::string, size_t>,
        MODALITY_COUNT
    > mapping_;

};
