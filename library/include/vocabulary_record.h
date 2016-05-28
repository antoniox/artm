#pragma once

#include <fstream>
#include <string>

#include "types.h"


struct VocabularyRecord {
    Modality modality;
    std::string token;
    size_t token_id;

    std::istream & load(std::istream & input);
    void save(std::ostream & output) const;
};
