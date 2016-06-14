#pragma once

#include <fstream>
#include <string>

#include "types.h"


struct VocabularyRecord {
    Modality modality;
    std::string token;
    id_type token_id;

    std::istream & load(std::istream & input);
    void save(std::ostream & output) const;
    void save_string(std::ostream & output) const;
};
