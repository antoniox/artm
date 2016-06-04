#include "types.h"


Token::Token(const Modality & modality_, id_type id_)
    : modality(modality_), id(id_) {
}


Edge::Edge(std::vector<Token> && tokens_, const Type & type_, id_type id_)
    : tokens(std::move(tokens_)), type(type_), id(id_) {

    hash_ = (id << 4) | type;
}


bool Edge::operator == (const Edge & other) const {
    return hash_ ==  other.hash_;
}


EdgeEntry::EdgeEntry(Edge && edge_, size_type entries_)
    : edge(std::move(edge_)), entries(entries_) {
}


bool TokenProbability::operator < (const TokenProbability & other) const {
    // for correct order of comparison for pq
    return value > other.value;
}
