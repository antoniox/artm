#include <iostream>

#include "document_record.h"
#include "utils.h"


std::istream & DocumentRecord::load(std::istream & input) {
    READ_BINARY(input, document_id);
    READ_BINARY(input, word_id);
    READ_BINARY(input, nick_id);
    READ_BINARY(input, date_id);
    READ_BINARY(input, entries);

    return input;
}


void DocumentRecord::save(std::ostream & output) const {
    WRITE_BINARY(output, document_id);
    WRITE_BINARY(output, word_id);
    WRITE_BINARY(output, nick_id);
    WRITE_BINARY(output, date_id);
    WRITE_BINARY(output, entries);
}


void DocumentRecord::save_string(std::ostream & output) const {
    output <<
        document_id << '\t' <<
        word_id << '\t' <<
        nick_id << '\t' <<
        date_id << '\t' <<
        entries << '\n';
}
