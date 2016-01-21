#pragma once

#include <fstream>


struct DocumentRecord {
    size_t document_id;
    size_t word_id;
    size_t nick_id;
    size_t date_id;
    size_t entries;

    void load(std::istream & input);
    void save(std::ostream & output) const;

    static void save_header(std::ostream & output);
};
