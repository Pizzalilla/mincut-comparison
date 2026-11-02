// generators.cpp — input families for the experiments.
//
// Written by Claude (AI) at Kartikay's request on 2026-11-02, because the
// generators were blocking the experiments. Originally assigned to Ethan.

#include "generators.hpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

namespace {

// Relabels the vertices with a random permutation, so the construction order
// (cluster 0 first, the cycle in order, ...) is not visible to the algorithms.
Graph shuffled(Graph g, std::mt19937_64& rng) {
    std::vector<int> label(g.n);
    std::iota(label.begin(), label.end(), 0);
    std::shuffle(label.begin(), label.end(), rng);
    for (Edge& e : g.edges) {
        e.u = label[e.u];
        e.v = label[e.v];
    }
    std::shuffle(g.edges.begin(), g.edges.end(), rng);
    return g;
}

// Connects vertices first..first+count-1 with a random spanning tree: each
// vertex attaches to a uniformly random earlier one.
void addSpanningTree(Graph& g, int first, int count, std::mt19937_64& rng) {
    for (int i = 1; i < count; ++i) {
        std::uniform_int_distribution<int> pick(0, i - 1);
        g.edges.push_back({first + i, first + pick(rng), 1});
    }
}

// Adds an edge between each pair in first..first+count-1 with probability p.
// A pair that is already a tree edge may get a parallel edge, which the
// interface allows.
void addRandomEdges(Graph& g, int first, int count, double p, std::mt19937_64& rng) {
    std::bernoulli_distribution coin(p);
    for (int u = 0; u < count; ++u)
        for (int v = u + 1; v < count; ++v)
            if (coin(rng)) g.edges.push_back({first + u, first + v, 1});
}

Graph randomGraph(int n, double p, std::uint64_t seed) {
    std::mt19937_64 rng(seed);
    Graph g{n, {}};
    addSpanningTree(g, 0, n, rng);
    addRandomEdges(g, 0, n, p, rng);
    return shuffled(std::move(g), rng);
}

}  // namespace

Graph randomDense(int n, std::uint64_t seed) {
    return randomGraph(n, 0.5, seed);
}

Graph randomSparse(int n, std::uint64_t seed) {
    return randomGraph(n, std::min(1.0, 6.0 / n), seed);
}

Graph plantedCut(int n, std::uint64_t seed) {
    std::mt19937_64 rng(seed);
    Graph g{n, {}};
    const int left = n / 2, right = n - left;
    for (auto [first, count] : {std::pair{0, left}, std::pair{left, right}}) {
        addSpanningTree(g, first, count, rng);
        addRandomEdges(g, first, count, 0.8, rng);
    }
    std::uniform_int_distribution<int> pickLeft(0, left - 1), pickRight(left, n - 1);
    for (int i = 0; i < 2; ++i) g.edges.push_back({pickLeft(rng), pickRight(rng), 1});
    return shuffled(std::move(g), rng);
}

Graph cycleGraph(int n, std::uint64_t seed) {
    std::mt19937_64 rng(seed);
    Graph g{n, {}};
    for (int v = 0; v < n; ++v) g.edges.push_back({v, (v + 1) % n, 1});
    return shuffled(std::move(g), rng);
}

Graph ringOfCliques(int n, std::uint64_t seed) {
    std::mt19937_64 rng(seed);
    Graph g{n, {}};
    // Cliques of at least 7 vertices (3 cliques if n < 21, but never more
    // cliques than vertices, so none is empty); the remainder is spread one
    // vertex at a time over the first cliques.
    const int cliques = std::min(n, std::max(3, n / 7));
    std::vector<int> start(cliques + 1, 0);
    for (int c = 0; c < cliques; ++c)
        start[c + 1] = start[c] + n / cliques + (c < n % cliques ? 1 : 0);
    for (int c = 0; c < cliques; ++c)
        for (int u = start[c]; u < start[c + 1]; ++u)
            for (int v = u + 1; v < start[c + 1]; ++v) g.edges.push_back({u, v, 1});
    // Link clique c to clique c+1 (and the last back to the first). Link 0 is
    // the weak one.
    for (int c = 0; c < cliques; ++c) {
        const int next = (c + 1) % cliques;
        std::uniform_int_distribution<int> here(start[c], start[c + 1] - 1);
        std::uniform_int_distribution<int> there(start[next], start[next + 1] - 1);
        const int links = c == 0 ? 2 : 3;
        for (int i = 0; i < links; ++i) g.edges.push_back({here(rng), there(rng), 1});
    }
    return shuffled(std::move(g), rng);
}
