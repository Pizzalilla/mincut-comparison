// generators.hpp — input families for the experiments.
//
// Primary author: Ethan
//
// Planned families (see README): random graphs of varying density, two dense
// clusters joined by a planted sparse cut, cycles, and graphs with many
// near-minimum cuts. Every generator should take an explicit seed so that
// every algorithm runs on exactly the same instances.

#ifndef MINCUT_GENERATORS_HPP
#define MINCUT_GENERATORS_HPP

#include "graph.hpp"
#include <cstdint>


Graph generateRandomGraph(int n, double p, long long max_weight, std::uint64_t seed);
Graph generateBarbellGraph(int n, long long max_weight, std::uint64_t seed); 
Graph generateCycleGraph(int n, long long weight);
Graph generateManyCutsGraph(int n, int k, long long max_weight, long long bridge_weight, std::uint64_t seed);

#endif  // MINCUT_GENERATORS_HPP
