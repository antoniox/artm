#include "constants.h"
#include "document_record.h"


void DocumentRecord::load(std::istream & input) {
    input >>
        document_id >>
        word_id >> nick_id >> date_id >>
        entries;
}


void DocumentRecord::save(std::ostream & output) const {
    output << 
        document_id << DELIMITER <<
        word_id << DELIMITER <<
        nick_id << DELIMITER <<
        date_id << DELIMITER <<
        entries << std::endl;
}


void DocumentRecord::save_header(std::ostream & output) {
    output << 
        "document_id" << DELIMITER <<
        "word_id" << DELIMITER <<
        "nick_id" << DELIMITER <<
        "date_id" << DELIMITER <<
        "entries" << std::endl;
}
