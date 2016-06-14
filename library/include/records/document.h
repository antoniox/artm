#pragma once

#include <fstream>

#include "types.h"


struct DocumentRecord {
    id_type document_id;
    id_type word_id;
    id_type nick_id;
    id_type date_id;
    size_type entries;

    bool is_valid_;

    std::istream & load(std::istream & input);
    void save(std::ostream & output) const;
    void save_string(std::ostream & output) const;
};
