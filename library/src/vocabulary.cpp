#include <unordered_map>

#include "vocabulary.h"
#include "vocabulary_record.h"
#include "utils.h"


void Vocabulary::load(std::istream & input) {
    // skip header
    skip_line(input);

    VocabularyRecord record;

    while (input) {
        record.load(input);
        mapping_[record.modality][record.token] = record.token_id;
    }
}


void Vocabulary::save(std::ostream & output) const {
    VocabularyRecord record;
    record.save_header(output);

    Modality modality = static_cast<Modality>(0);

    for (auto & inner_mapping : mapping_) {
        record.modality = modality;

        for (
            auto inner_iterator = inner_mapping.begin();
            inner_iterator != inner_mapping.end();
            ++inner_iterator
        ) {

            auto & token = inner_iterator->first;
            auto & token_id = inner_iterator->second;

            record.token = token;
            record.token_id = token_id;

            record.save(output);
        }

        // XXX: set next function
        modality = static_cast<Modality>(modality + 1);
    }
}


size_t Vocabulary::get_id(const Modality & modality, const std::string & token) {
    auto & inner_mapping = mapping_[modality];

    size_t new_id = inner_mapping.size();
    auto pair = inner_mapping.emplace(token, new_id);

    return pair.first->second; 
}
