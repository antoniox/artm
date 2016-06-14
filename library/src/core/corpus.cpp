#include "core/corpus.h"

Corpus::Corpus() {}

Corpus::Corpus(CorpusImpl * corpus_impl_ptr)
    : corpus_impl_ptr_(corpus_impl_ptr) {}


std::shared_ptr<CorpusIterator> Corpus::iterator() {
    return corpus_impl_ptr_->iterator();
}


const Vocabulary & Corpus::vocabulary() const {
    return corpus_impl_ptr_->vocabulary();
}


bool CorpusIterator::equal(const CorpusIterator & other) const {
    return false;
}
