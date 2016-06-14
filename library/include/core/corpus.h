#pragma once

#include <memory>

#include <boost/iterator/iterator_facade.hpp>

#include "core/document.h"
#include "core/vocabulary.h"


class CorpusImpl;
class CorpusIterator;


class Corpus {
private:
    std::shared_ptr<CorpusImpl> corpus_impl_ptr_;

public:
    Corpus();
    Corpus(CorpusImpl * corpus_impl_ptr);

    std::shared_ptr<CorpusIterator> iterator();
    const Vocabulary & vocabulary() const;
};


class CorpusImpl {
public:
    virtual std::shared_ptr<CorpusIterator> iterator() = 0;
    virtual const Vocabulary & vocabulary() const = 0;

    virtual ~CorpusImpl() {}
};


class CorpusIterator
    : public boost::iterator_facade<
      CorpusIterator,
      Document,
      boost::forward_traversal_tag,
      const Document &
    > {

public:
    virtual bool has_more() const = 0;
    virtual ~CorpusIterator() {}

protected:
    friend class boost::iterator_core_access;

    virtual void increment() = 0;
    virtual bool equal(const CorpusIterator & other) const;
    virtual const Document & dereference() const = 0;
};
