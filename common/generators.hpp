// generators.hpp — input families for the experiments.
//
// Written by Claude (AI) at Kartikay's request on 2026-11-02, because the
// generators were blocking the experiments. Originally assigned to Ethan.
//
// Every generator returns a connected graph on vertices 0..n-1 (n >= 2) with
// positive integer weights, and is deterministic in its seed, so every
// algorithm runs on exactly the same instances. Vertex labels are shuffled so
// that no algorithm benefits from the order vertices happen to be built in.

#ifndef MINCUT_GENERATORS_HPP
#define MINCUT_GENERATORS_HPP

#include <cstdint>

#include "graph.hpp"

// G(n, 1/2) with unit weights, plus a random spanning tree for connectivity.
// The minimum cut is usually the lowest-degree vertex.
Graph randomDense(int n, std::uint64_t seed);

// G(n, p) with p = 6/n (average degree about 6) plus a random spanning tree,
// unit weights. Few edges per vertex, so the minimum cut is small.
Graph randomSparse(int n, std::uint64_t seed);

// Two clusters of about n/2 vertices, each G(n/2, 0.8), joined by exactly 2
// unit edges. For n >= 20 the planted 2-edge cut is the unique minimum cut.
Graph plantedCut(int n, std::uint64_t seed);

// A single cycle with unit weights. Minimum cut 2, and every pair of cycle
// edges is a minimum cut: n(n-1)/2 of them.
Graph cycleGraph(int n, std::uint64_t seed);

// A ring of cliques of 7 vertices. Neighbouring cliques are joined by 3 unit
// edges, except one weak link with 2. Cutting the weak link plus any other
// link gives the minimum cut, 5; cutting any two strong links gives a
// near-minimum cut of 6, and there are many of those. Intended for n >= 21;
// smaller n gives 3 smaller cliques, whose minimum cut may be a single vertex.
Graph ringOfCliques(int n, std::uint64_t seed);

#endif  // MINCUT_GENERATORS_HPP
