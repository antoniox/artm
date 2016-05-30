#include <iostream>

#include "vocabulary_record.h"


int main(void) {
    std::ios_base::sync_with_stdio(false);

    VocabularyRecord record;

    while (record.load(std::cin)) {
        record.save_string(std::cout);
    }
}
