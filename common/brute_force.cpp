// brute_force.cpp — exact minimum cut by trying every bipartition (small n).
//
// Primary author: Brian

#include "brute_force.hpp"

#include <stdexcept>

long long bruteForceMinCut(const Graph& g, std::vector<int>& side,
                           std::uint64_t seed) {
    (void)g, (void)side, (void)seed;
    throw std::logic_error("bruteForceMinCut: not implemented yet");
}
