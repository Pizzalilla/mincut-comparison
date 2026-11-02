// karger.cpp — Karger's randomised contraction algorithm, repeated.
//
// Primary author: Brian

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "algorithms.hpp"

long long kargerMinCut(const Graph& g, std::vector<int>& side,
                       std::uint64_t seed) {
    (void)g, (void)side, (void)seed;
    if (g.n < 2) {
        throw std::invalid_argument("Graph must have at least 2 vertices");
    }

    for (const Edge& e : g.edges) {
        if (e.u < 0 || e.u >= g.n ||
            e.v < 0 || e.v >= g.n) {
            throw std::invalid_argument("Invalid edge endpoint");
        }

        if (e.w < 0) {
            throw std::invalid_argument(
                "Edge weights must be non-negative");
        }
    }
    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<long double> dist(0.0L, 1.0L);

    // Assign each positive-weight edge a random key.
    std::vector<std::pair<long double, std::size_t>> edgeOrder;
    for (std::size_t i = 0; i < g.edges.size(); ++i) {
        const Edge& e = g.edges[i];
        if (e.w == 0 || e.u == e.v) {
            continue;
        }

        long double u;
        do {
            u = dist(rng);
        } while (u == 0.0L);
        long double key = -std::log(u) / e.w;
        edgeOrder.push_back({key, i});
    }

    // Process edges in increasing random-key order.
    std::sort(edgeOrder.begin(), edgeOrder.end());

    // Initialise Union-Find.
    std::vector<int> parent(g.n);
    std::vector<int> size(g.n, 1);
    for (int v = 0; v < g.n; ++v) {
        parent[v] = v;
    }

    // Find the root of a component with path compression.
    auto find = [&parent](int x) {
        int root = x;
        while (parent[root] != root) {
            root = parent[root];
        }
        while (parent[x] != x) {
            int next = parent[x];
            parent[x] = root;
            x = next;
        }
        return root;
    };
    int components = g.n;

    // Contract edges until two components remain.
    for (const auto& entry : edgeOrder) {
        if (components == 2) {
            break;
        }

        const Edge& e = g.edges[entry.second];
        int rootU = find(e.u);
        int rootV = find(e.v);

        // Already in the same component (self-loop after contraction).
        if (rootU == rootV) {
            continue;
        }

        // Union by size.
        if (size[rootU] < size[rootV]) {
            std::swap(rootU, rootV);
        }

        parent[rootV] = rootU;
        size[rootU] += size[rootV];
        --components;
    }

    // Construct the final two-sided partition.
    side.assign(g.n, 0);
    int firstRoot = find(0);
    for (int v = 0; v < g.n; ++v) {
        side[v] = (find(v) == firstRoot) ? 0 : 1;
    }

    // Calculate the cut weight using the original graph.
    long long cutWeight = 0;
    for (const Edge& e : g.edges) {
        if (side[e.u] != side[e.v]) {
            cutWeight += e.w;
        }
    }
    return cutWeight;
}