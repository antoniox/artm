#include "batch.h"


BatchParser::BatchParser(std::istream & input, size_type batch_size)
    : batch_size_(batch_size),
      document_parser_(input),
      has_more_(document_parser_.has_more()) {
}


bool BatchParser::has_more() const {
    return has_more_;
}


void BatchParser::parse(Batch & batch) {
    auto & documents = batch.documents;
    documents.clear();

    Document document;

    while (document_parser_.has_more() && documents.size() < batch_size_) {
        document_parser_.parse(document);
        documents.push_back(document);
    }

    has_more_ = document_parser_.has_more();
}
