#pragma once

#include <fstream>
#include <vector>

#include "document.h"
#include "types.h"
#include "utils.h"


struct Batch {
    std::vector<Document> documents;
};


class BatchParser {
public:
    BatchParser(std::istream & input, size_type batch_size);
    bool has_more() const;
    void parse(Batch & batch);

private:
    size_type batch_size_;
    DocumentParser document_parser_;
    bool has_more_;
};
