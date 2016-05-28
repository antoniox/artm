#pragma once

#include <fstream>

#include "types.h"


struct DocumentRecord {
    id_type document_id;
    id_type word_id;
    id_type nick_id;
    id_type date_id;
    size_type entries;

    std::istream & load(std::istream & input);
    void save(std::ostream & output) const;
};
