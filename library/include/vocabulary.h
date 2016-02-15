#pragma once

#include <array>
#include <fstream>
#include <string>
#include <unordered_map>

#include "types.h"


class Vocabulary {
public:
    void load(std::istream & input);
    void save(std::ostream & output) const;

    id_type token_id(const Modality & modality, const std::string & token);
    id_type token_id(const Modality & modality, const std::string & token) const;
    const std::string & token(const Modality & modality, id_type token_id) const;

    size_type modality_size(const Modality & modality) const;

private:
    std::array<
        std::unordered_map<std::string, id_type>, 
        MODALITY_COUNT
    > token_id_map_;

    std::array<
        std::unordered_map<id_type, std::string>, 
        MODALITY_COUNT
    > id_token_map_;
};
