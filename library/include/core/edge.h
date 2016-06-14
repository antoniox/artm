#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include "core/token.h"
#include "utils/types.h"


class Edge {
private:
    static std::unordered_map<id_type, size_type> edges_count_;

    id_type type_;
    id_type id_;

    std::vector<Token> tokens_;

    // hash value precomputed upon creation
    size_t hash_;

public:
    Edge(id_type type, const std::vector<Token> & tokens);

    id_type type() const;
    id_type id() const;

    size_t hash() const;

    std::vector<Token>::const_iterator begin() const;
    std::vector<Token>::const_iterator end() const;

    bool operator == (const Edge & other) const;
};


namespace std {
    template <> struct hash<Edge> {
        size_t operator ()(const Edge & edge) const {
            return edge.hash();
        }
    };
}


class EdgeEntry {
private:
    Edge edge_;
    size_type entries_;

public:
    EdgeEntry(Edge && edge, size_type entries);

    const Edge & edge() const;
    size_type entries() const;
};
