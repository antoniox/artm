#pragma once

#include <fstream>
#include <string>

#include "core/corpus.h"
#include "core/document.h"
#include "core/vocabulary.h"
#include "utils/types.h"


class LiveJournalCorpusIterator;

class LiveJournalCorpusImpl
    : public CorpusImpl {

private:
    std::string filename_;
    Vocabulary vocabulary_;

    friend class LiveJournalCorpusIterator;

public:
    LiveJournalCorpusImpl(const std::string & filename);

    virtual std::shared_ptr<CorpusIterator> iterator();
    virtual const Vocabulary & vocabulary() const;
};


class LiveJournalCorpusIterator
    : public CorpusIterator {

private:
    LiveJournalCorpusImpl & corpus_;
    std::ifstream input_;
    Document current_document_;
    bool has_more_;

    bool parse_document();

    virtual void increment();
    virtual const Document & dereference() const;

public:
    LiveJournalCorpusIterator(LiveJournalCorpusImpl & corpus);
    virtual bool has_more() const;
};
