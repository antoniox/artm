#include <unordered_map>

#include "utils.h"
#include "vocabulary.h"
#include "vocabulary_record.h"


void Vocabulary::load(std::istream & input) {
    // skip header
    skip_line(input);

    VocabularyRecord record;

    while (input) {
        record.load(input);
        token_id_map_[record.modality][record.token] = record.token_id;
    }
}


void Vocabulary::save(std::ostream & output) const {
    VocabularyRecord record;
    record.save_header(output);

    FOR(id_type, modality_id, MODALITY_COUNT) {
        Modality modality = static_cast<Modality>(modality_id);
        auto & modality_slice = token_id_map_[modality];

        record.modality = modality;

        for (
            auto iterator = modality_slice.begin();
            iterator != modality_slice.end();
            ++iterator
        ) {
            auto & token = iterator->first;
            auto & token_id = iterator->second;

            record.token = token;
            record.token_id = token_id;

            record.save(output);
        }
    }
}


id_type Vocabulary::token_id(const Modality & modality, const std::string & token) {
    auto & modality_slice = token_id_map_[modality];

    size_t new_id = modality_slice.size();
    auto pair = modality_slice.emplace(token, new_id);

    if (pair.second) {
        id_token_map_[modality][new_id] = token;
    }

    return pair.first->second; 
}


id_type Vocabulary::token_id(const Modality & modality, const std::string & token) const {
    auto & modality_slice = token_id_map_[modality];
    auto iterator = modality_slice.find(token);
    return iterator->second;
}


const std::string & Vocabulary::token(const Modality & modality, id_type token_id) const {
    auto & modality_slice = id_token_map_[modality];
    auto iterator = modality_slice.find(token_id);
    return iterator->second;
}


size_type Vocabulary::modality_size(const Modality & modality) const {
    return token_id_map_[modality].size();
}
