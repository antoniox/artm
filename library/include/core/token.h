#pragma once

#include "utils/types.h"


class Token {
private:
    id_type modality_;
    id_type id_;

public:
    Token(id_type modality, id_type id);

    id_type modality() const;
    id_type id() const;

    void set_modality(id_type modality);
    void set_id(id_type id);
};
