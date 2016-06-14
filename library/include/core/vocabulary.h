#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "core/token.h"
#include "utils/types.h"


class Vocabulary {
private:
    std::unordered_map<id_type, std::string> modality_names_;

    std::unordered_map<
        id_type, std::unordered_map<std::string, id_type> 
    > token_to_id_;

    std::unordered_map<
        id_type, std::unordered_map<id_type, std::string> 
    > id_to_token_;

public:
    Vocabulary();
    Vocabulary(const Vocabulary & other);
    Vocabulary(const std::string & filename);

    void save(const std::string & filename) const;
    void save_text(const std::string & filename) const;

    void load(const std::string & filename);

    id_type token_id(id_type modality, const std::string & token);
    id_type token_id(id_type modality, const std::string & token) const;

    const std::string & token(id_type modality, id_type token_id) const;
    const std::string & token(const Token & token) const;

    size_type modality_size(id_type modality) const;
    void set_modality_names(const std::unordered_map<id_type, std::string> & modality_names);
};
