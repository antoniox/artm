#pragma once

#include <fstream>
#include <vector>

#include "document_record.h"
#include "types.h"


struct Document {
    std::vector<EdgeEntry> edge_entries;
};


class DocumentParser {
public:
    DocumentParser(std::istream & input);
    bool has_more() const;
    void parse(Document & document);

private:
    std::istream & input_; 
    DocumentRecord cached_document_record_;
    bool has_more_;
};
