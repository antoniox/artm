#pragma once

#include <array>
#include <fstream>
#include <string>
#include <unordered_map>

#include "modality.h"
#include "typedefs.h"


class Vocabulary {
public:
    void load(std::istream & input);
    void save(std::ostream & output) const;

    id_type get_id(const Modality & modality, const std::string & token);
    size_type modality_size(const Modality & modality) const;

private:
    std::array<
        std::unordered_map<std::string, id_type>,
        MODALITY_COUNT
    > mapping_;

};
