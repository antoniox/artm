#include "vocabulary_record.h"
#include "utils.h"


std::istream & VocabularyRecord::load(std::istream & input) {
    unsigned int tmp;

    READ_BINARY(input, tmp);

    if (! input) {
        return input;
    }

    modality = static_cast<Modality>(tmp);

    size_t size;
    READ_BINARY(input, size);
    char * tmp_string = new char [size];
    input.read(tmp_string, size);
    token = tmp_string;
    delete [] tmp_string;

    READ_BINARY(input, token_id);

    return input;
}


void VocabularyRecord::save(std::ostream & output) const {
    unsigned int tmp = modality;

    WRITE_BINARY(output, tmp);

    size_t size = token.size() + 1;
    WRITE_BINARY(output, size);
    output.write(token.c_str(), size);

    WRITE_BINARY(output, token_id);
}
