#include "core/batch.h"


Batch::Batch(CorpusIterator & corpus_iterator, size_type batch_size) { 
    documents_.reserve(batch_size);
    while (corpus_iterator.has_more() && size() < batch_size) {
        documents_.push_back(*corpus_iterator);
        ++corpus_iterator;
    }
}


size_type Batch::size() const {
    return documents_.size();
}


std::vector<Document>::const_iterator Batch::begin() const {
    return documents_.begin();
}


std::vector<Document>::const_iterator Batch::end() const {
    return documents_.end();
}
