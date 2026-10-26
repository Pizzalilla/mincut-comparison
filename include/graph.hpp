// graph.hpp — shared graph representation and min-cut interface.
//
// Every algorithm, the brute-force checker, the generators and the benchmark
// harness use these types. Change this file only after the whole group agrees.

#ifndef MINCUT_GRAPH_HPP
#define MINCUT_GRAPH_HPP

#include <cstdint>
#include <vector>

struct Edge {
    int u, v;        // endpoints, 0 <= u, v < n, u != v
    long long w;     // non-negative weight; parallel edges are allowed
};

struct Graph {
    int n = 0;                 // vertices are 0..n-1
    std::vector<Edge> edges;
};

// Every min-cut algorithm has this signature, so the tests and the benchmark
// harness can treat them interchangeably.
//
//   g     connected, undirected, n >= 2
//   side  output: resized to g.n, side[v] is 0 or 1, both sides non-empty
//   seed  RNG seed (deterministic algorithms ignore it)
//
// Returns the weight of the cut described by side. Randomised algorithms may
// return a cut that is not minimum.
using MinCutFn = long long (*)(const Graph& g, std::vector<int>& side,
                               std::uint64_t seed);

// Total weight of edges crossing the cut described by side.
inline long long cutWeight(const Graph& g, const std::vector<int>& side) {
    long long total = 0;
    for (const Edge& e : g.edges)
        if (side[e.u] != side[e.v]) total += e.w;
    return total;
}

#endif  // MINCUT_GRAPH_HPP
