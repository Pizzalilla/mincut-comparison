// brute_force.hpp — exact minimum cut by trying every bipartition (small n).
//
// Primary author: Brian

#ifndef MINCUT_BRUTE_FORCE_HPP
#define MINCUT_BRUTE_FORCE_HPP

#include "graph.hpp"

// Same contract as MinCutFn, so it can serve as ground truth in the tests.
// Only practical for small graphs (around n <= 16).
long long bruteForceMinCut(const Graph& g, std::vector<int>& side,
                           std::uint64_t seed);

#endif  // MINCUT_BRUTE_FORCE_HPP
