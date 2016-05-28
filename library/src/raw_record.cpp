#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include <pcrecpp.h>

#include "raw_record.h"


std::istream & RawRecord::load(std::istream & input) {
    static std::string buffer;
    static boost::escaped_list_separator<char> splitter('\\', ';', '\"');

    std::getline(input, buffer);

    if (! input) {
        return input;
    }

    boost::algorithm::trim_right(buffer);

    boost::tokenizer<boost::escaped_list_separator<char>>
        tokenizer(buffer, splitter);

    auto ptr = tokenizer.begin();

    original_document_id = *ptr++;
    text = *ptr++;
    nick = *ptr++;
    url = *ptr++;
    date = *ptr++;
    time = *ptr;

    make_document_id();

    return input;
}


DocumentRecordTokenizer RawRecord::tokenize(Vocabulary & vocabulary) {
    return DocumentRecordTokenizer(*this, vocabulary);
}


void RawRecord::make_document_id() {
    document_id = last_document_id_;
    ++last_document_id_;
}


size_t RawRecord::last_document_id_ = 0;


DocumentRecordTokenizer::DocumentRecordTokenizer(
    const RawRecord & raw_record,
    Vocabulary & vocabulary
) :
    raw_record_(raw_record),
    vocabulary_(vocabulary) {

    _fill_word_entries();
}


DocumentRecordIterator DocumentRecordTokenizer::begin() {
    return DocumentRecordIterator(
        *this, word_entries_.begin()
    );
}

DocumentRecordIterator DocumentRecordTokenizer::end() {
    return DocumentRecordIterator(
        *this, word_entries_.end()
    );
}


void DocumentRecordTokenizer::_fill_word_entries() {
    std::vector<std::string> words;

    boost::algorithm::split(
        words, raw_record_.text,
        boost::algorithm::is_any_of("\t ")
    );

    for (auto & word : words) {
        auto pair = word_entries_.emplace(word, 0);
        pair.first->second += 1;
    }
}


bool is_good_word(const std::string & word) {
    // XXX: input file MUST be in UTF-8
    static pcrecpp::RE good_word("[а-я]{3,}", pcrecpp::UTF8());
    return good_word.FullMatch(word);
}


DocumentRecordIterator::DocumentRecordIterator(
    const DocumentRecordTokenizer & tokenizer,
    std::unordered_map<std::string, size_t>::const_iterator word_iterator
) :
    tokenizer_(tokenizer),
    word_iterator_(word_iterator) {

    auto & raw_record = tokenizer_.raw_record_;

    document_record_.document_id = raw_record.document_id;
    document_record_.nick_id = tokenizer_.vocabulary_.token_id(NICK, raw_record.nick);
    document_record_.date_id = tokenizer_.vocabulary_.token_id(DATE, raw_record.date);

    auto begin = tokenizer_.word_entries_.begin();
    document_record_.word_id = tokenizer_.vocabulary_.token_id(WORD, begin->first);
    document_record_.entries = begin->second;
}
    

void DocumentRecordIterator::increment() {
    auto end = tokenizer_.word_entries_.end();

    do {
        ++word_iterator_;
    } while (
        ! (
            word_iterator_ == end ||
            is_good_word(word_iterator_->first)
        )
    );

    if (word_iterator_ != end) {
        auto & word = word_iterator_->first;
        auto & entries = word_iterator_->second;

        document_record_.word_id = tokenizer_.vocabulary_.token_id(WORD, word);
        document_record_.entries = entries;
    }
}


const DocumentRecord & DocumentRecordIterator::dereference() const {
    return document_record_;
}


bool DocumentRecordIterator::equal(const DocumentRecordIterator & other) const {
    return word_iterator_ == other.word_iterator_;
}
