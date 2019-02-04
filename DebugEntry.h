#pragma once

#include "Query.h"

struct DebugEntry {
    const Query *query;
    unsigned long parent_ordinal;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;

    DebugEntry() {}
    DebugEntry(const Query *query, unsigned long parent_ordinal, std::chrono::time_point<std::chrono::high_resolution_clock> &&start_time)
            : query(query), parent_ordinal(parent_ordinal), start_time(start_time) {}
};

class DebugStack {
    std::vector<DebugEntry> realDebugStack;

public:
    const std::vector<DebugEntry> &entries() { return realDebugStack; }

    unsigned long start(const Query *query, unsigned long parentOrdinal) {
        unsigned long ordinal = realDebugStack.size();
        realDebugStack.emplace_back(query, parentOrdinal, std::chrono::high_resolution_clock::now());
        return ordinal;
    }

    void stop(unsigned long ordinal) {
        realDebugStack[ordinal].end_time = std::chrono::high_resolution_clock::now();
    }
};
