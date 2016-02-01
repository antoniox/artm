#pragma once

#include <fstream>
#include <vector>

#include "document_record.h"
#include "modality.h"
#include "typedefs.h"


enum Type {
    FIRST, TYPE_COUNT
};


struct Token {
    Modality modality;
    id_type id;

    Token(const Modality & modality, id_type id);
};


struct Edge {
    std::vector<Token> tokens;
    Type type;
    id_type id;

    Edge(std::vector<Token> && tokens, const Type & type, id_type id);
};


struct EdgeEntry {
    Edge edge;
    size_type entries;

    EdgeEntry(Edge && edge, size_type entries);
};


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
