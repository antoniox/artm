#include "constants.h"
#include "vocabulary_record.h"


void VocabularyRecord::load(std::istream & input) {
    unsigned int tmp = 0;

    // XXX: take DELIMITER into account
    input >> tmp >> token >> token_id;
    modality = static_cast<Modality>(tmp);
}


void VocabularyRecord::save(std::ostream & output) const {
    output <<
        modality << DELIMITER <<
        token << DELIMITER <<
        token_id << std::endl;
}


void VocabularyRecord::save_header(std::ostream & output) {
    output <<
        "modality" << DELIMITER <<
        "token" << DELIMITER <<
        "token_id" << std::endl;
}
