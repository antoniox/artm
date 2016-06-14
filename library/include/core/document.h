#pragma once

#include <vector>

#include "core/edge.h"


class Document {
private:
    std::vector<EdgeEntry> edge_entries_;

public:
    void add_edge_entry(EdgeEntry && edge_entry);
    void clear();

    std::vector<EdgeEntry>::const_iterator begin() const;
    std::vector<EdgeEntry>::const_iterator end() const;
};
