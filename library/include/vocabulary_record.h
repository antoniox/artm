#pragma once

#include <fstream>
#include <string>

#include "modality.h"


struct VocabularyRecord {
    Modality modality;
    std::string token;
    size_t token_id;

    void load(std::istream & input);
    void save(std::ostream & output) const;

    static void save_header(std::ostream & output);
};
