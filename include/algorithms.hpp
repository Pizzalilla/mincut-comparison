// algorithms.hpp — the three min-cut algorithms under comparison.
//
// Each function follows the MinCutFn contract in graph.hpp. Authors may add
// extra entry points in their own module header (for example a single
// Karger run, or a trial count), but these three must keep this signature.

#ifndef MINCUT_ALGORITHMS_HPP
#define MINCUT_ALGORITHMS_HPP

#include "graph.hpp"

// Primary author: Kartikay — stoer_wagner/stoer_wagner.cpp
long long stoerWagnerMinCut(const Graph& g, std::vector<int>& side,
                            std::uint64_t seed);

// Primary author: Brian — karger/karger.cpp
long long kargerMinCut(const Graph& g, std::vector<int>& side,
                       std::uint64_t seed);

long long kargerSingleRun(const Graph& g, std::vector<int>& side, std::uint64_t seed);

// Primary author: Ethan — karger_stein/karger_stein.cpp
long long kargerSteinMinCut(const Graph& g, std::vector<int>& side,
                            std::uint64_t seed);

#endif  // MINCUT_ALGORITHMS_HPP
