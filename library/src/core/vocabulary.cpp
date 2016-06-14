#include <fstream>

#include "core/vocabulary.h"
#include "utils/misc.h"


Vocabulary::Vocabulary() {}


Vocabulary::Vocabulary(const Vocabulary & other)
    : modality_names_(other.modality_names_),
      token_to_id_(other.token_to_id_),
      id_to_token_(other.id_to_token_) {}


Vocabulary::Vocabulary(const std::string & filename) {
    load(filename);
}


void Vocabulary::save(const std::string & filename) const {
    std::ofstream output(filename);

    size_type modality_count = token_to_id_.size();

    write_binary(output, modality_count);

    for (auto & modality_pair : token_to_id_) {
        auto modality_id = modality_pair.first;
        auto & tokens = modality_pair.second;

        std::string modality_name;

        auto iterator = modality_names_.find(modality_id);
        if (iterator != modality_names_.end()) {
            modality_name = iterator->second;
        } else {
            modality_name = std::to_string(modality_id);
        }

        write_binary(output, modality_id);
        write_binary(output, modality_name);

        size_type tokens_count = tokens.size();
        write_binary(output, tokens_count);

        for (auto & token_pair : tokens) {
            write_binary(output, token_pair.first);
            write_binary(output, token_pair.second);
        }
    }
}


void Vocabulary::load(const std::string & filename) {
    std::ifstream input(filename);

    modality_names_.clear();
    token_to_id_.clear();
    id_to_token_.clear();

    size_type modality_count;

    read_binary(input, modality_count);

    FOR(modality_index, modality_count) {
        id_type modality_id;
        std::string modality_name;

        read_binary(input, modality_id);
        read_binary(input, modality_name);

        modality_names_[modality_id] = modality_name;

        size_type tokens_count;
        read_binary(input, tokens_count);

        auto & modality_token_to_id = token_to_id_[modality_id];
        auto & modality_id_to_token = id_to_token_[modality_id];

        FOR(token_index, tokens_count) {
            std::string token;
            size_type token_id;

            read_binary(input, token);
            read_binary(input, token_id);

            modality_token_to_id[token] = token_id;
            modality_id_to_token[token_id] = token;
        }
    }
}


void Vocabulary::set_modality_names(const std::unordered_map<id_type, std::string> & modality_names) {
    modality_names_ = modality_names;
}
