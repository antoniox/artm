#pragma once

#include <fstream>

#include "types.h"


struct PhiRecord {
    Type type;
    id_type topic_id;
    Modality modality;
    id_type token_id;
    float_type value;

    std::istream & load(std::istream & input);
    void save(std::ostream & output) const;
    void save_string(std::ostream & output) const;
};
