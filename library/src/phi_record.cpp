#include <iostream>

#include "phi_record.h"
#include "utils.h"


std::istream & PhiRecord::load(std::istream & input) {
    unsigned int tmp;

    READ_BINARY(input, tmp);

    if (! input) {
        return input;
    }

    type = static_cast<Type>(tmp);

    READ_BINARY(input, topic_id);

    READ_BINARY(input, tmp);
    modality = static_cast<Modality>(tmp); 

    READ_BINARY(input, token_id);
    READ_BINARY(input, value);

    return input;
}


void PhiRecord::save(std::ostream & output) const {
    unsigned int tmp = type;

    WRITE_BINARY(output, tmp);
    WRITE_BINARY(output, topic_id);

    tmp = modality;
    WRITE_BINARY(output, tmp);
    WRITE_BINARY(output, token_id);
    WRITE_BINARY(output, value);
}


void PhiRecord::save_string(std::ostream & output) const {
    output <<
        type << '\t' <<
        topic_id << '\t' <<
        modality << '\t' <<
        token_id << '\t' <<
        value << std::endl;
}
