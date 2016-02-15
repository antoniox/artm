#pragma once

#include <functional>
#include <vector>

#include <boost/functional/hash.hpp>


typedef unsigned int size_type;
typedef unsigned int id_type;
typedef float float_type;


enum Modality {
    WORD, NICK, DATE, MODALITY_COUNT
};


enum Type {
    FIRST, TYPE_COUNT
};


struct Token {
    Modality modality;
    id_type id;

    Token(const Modality & modality_, id_type id_);
};


struct Edge {
    std::vector<Token> tokens;
    Type type;
    id_type id;

    Edge(std::vector<Token> && tokens_, const Type & type_, id_type id_);
    bool operator == (const Edge & other) const;
};


namespace std {
    template <> struct hash<Edge> {
        size_t operator ()(const Edge & edge) const {
            size_t seed = 0;

            boost::hash_combine(seed, edge.type);
            boost::hash_combine(seed, edge.id);

            return seed;
        }
    };
}


struct EdgeEntry {
    Edge edge;
    size_type entries;

    EdgeEntry(Edge && edge_, size_type entries_);
};
