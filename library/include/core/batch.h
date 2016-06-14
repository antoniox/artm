#pragma once

#include <vector>

#include "core/document.h"
#include "core/corpus.h"
#include "utils/types.h"


class Batch {
private:
    std::vector<Document> documents_;

public:
    Batch(CorpusIterator & corpus_iterator, size_type batch_size);
    size_type size() const;

    std::vector<Document>::const_iterator begin() const;
    std::vector<Document>::const_iterator end() const;
};
