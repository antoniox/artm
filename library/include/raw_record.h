#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

#include <boost/iterator/iterator_facade.hpp>

#include "document_record.h"
#include "vocabulary.h"


class DocumentRecordTokenizer;
class DocumentRecordIterator;


struct RawRecord {
    size_t document_id;
    std::string original_document_id;
    std::string text;
    std::string nick;
    std::string url;
    std::string date;
    std::string time;

    void load(std::istream & input);
    DocumentRecordTokenizer tokenize(Vocabulary & vocabulary);

    void make_document_id();

private:
    static size_t last_document_id_;
};


class DocumentRecordTokenizer {
public:
    DocumentRecordTokenizer(
        const RawRecord & raw_record,
        Vocabulary & vocabulary
    );

    DocumentRecordIterator begin();
    DocumentRecordIterator end();

private:
    friend class DocumentRecordIterator;

    const RawRecord & raw_record_;
    Vocabulary & vocabulary_;
    std::unordered_map<std::string, size_t> word_entries_;

    void _fill_word_entries();
};


class DocumentRecordIterator
    : public boost::iterator_facade<
      DocumentRecordIterator,
      DocumentRecord,
      boost::forward_traversal_tag,
      const DocumentRecord &
    > {
public:
    explicit DocumentRecordIterator(
        const DocumentRecordTokenizer & tokenizer,
        std::unordered_map<std::string, size_t>::const_iterator word_iterator
    );

private:
    friend class boost::iterator_core_access;

    void increment();
    bool equal(const DocumentRecordIterator & other) const;
    const DocumentRecord & dereference() const;

    DocumentRecordTokenizer tokenizer_;
    std::unordered_map<std::string, size_t>::const_iterator word_iterator_;
    DocumentRecord document_record_;
};
