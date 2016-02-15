#include "types.h"


Token::Token(const Modality & modality_, id_type id_)
    : modality(modality_), id(id_) {
}


Edge::Edge(std::vector<Token> && tokens_, const Type & type_, id_type id_)
    : tokens(std::move(tokens_)), type(type_), id(id_) {
}


bool Edge::operator == (const Edge & other) const {
    return id == other.id && type == other.type;
}


EdgeEntry::EdgeEntry(Edge && edge_, size_type entries_)
    : edge(std::move(edge_)), entries(entries_) {
}
