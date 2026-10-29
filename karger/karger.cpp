// karger.cpp — Karger's randomised contraction algorithm, repeated.
//
// Primary author: Brian

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
    // throw std::logic_error("kargerMinCut: not implemented yet");
    if (g.n < 2) {
        throw std::invalid_argument("Graph must have at least 2 vertices");
    }

    for (const Edge& e : g.edges) {
        if (e.u < 0 || e.u >= g.n ||
            e.v < 0 || e.v >= g.n) {
            throw std::invalid_argument("Invalid edge endpoint");
        }
        if (e.w < 0) {
            throw std::invalid_argument("Edge weights must be non-negative");
        }
    }
    constexpr int TRIALS = 1000;
    std::mt19937_64 rng(seed);
    long long bestCut = std::numeric_limits<long long>::max();
    std::vector<int> bestSide;
    // Repeat the random contraction algorithm.
    for (int trial = 0; trial < TRIALS; ++trial) {
        // Each vertex initially represents its own component.
        std::vector<int> component(g.n);
        for (int v = 0; v < g.n; ++v) {
            component[v] = v;
        }
        int remaining = g.n;
        // Contract edges until two components remain.
        while (remaining > 2) {
            long double totalWeight = 0.0L;
            // Count the weights of edges between different components.
            for (const Edge& e : g.edges) {
                if (component[e.u] != component[e.v]) {
                    totalWeight += static_cast<long double>(e.w);
                }
            }
            // No positive-weight edge remains between components.
            if (totalWeight == 0.0L) {
                break;
            }

            // Select a crossing edge with probability proportional
            // to its weight.
            std::uniform_real_distribution<long double> dist(0.0L, totalWeight);
            long double target = dist(rng);
            long double cumulative = 0.0L;
            int u = -1;
            int v = -1;
            int lastU = -1;
            int lastV = -1;
            for (const Edge& e : g.edges) {
                if (component[e.u] == component[e.v] || e.w == 0) {
                    continue;
                }

                lastU = e.u;
                lastV = e.v;
                cumulative += static_cast<long double>(e.w);

                if (target < cumulative) {
                    u = e.u;
                    v = e.v;
                    break;
                }
            }
            // Fallback for floating-point rounding.
            if (u == -1) {
                u = lastU;
                v = lastV;
            }

            if (u == -1) {
                break;
            }

            // Merge v's component into u's component.
            int oldComponent = component[v];
            int newComponent = component[u];
            for (int x = 0; x < g.n; ++x) {
                if (component[x] == oldComponent) {
                    component[x] = newComponent;
                }
            }
            --remaining;
        }
        // Convert the remaining components into a cut.
        std::vector<int> currentSide(g.n);
        for (int v = 0; v < g.n; ++v) {
            currentSide[v] =
                (component[v] == component[0]) ? 0 : 1;
        }
        // Calculate the cut weight using the original edges.
        long long cutWeight = 0;
        for (const Edge& e : g.edges) {
            if (currentSide[e.u] != currentSide[e.v]) {
                cutWeight += e.w;
            }
        }
        // Save the best cut found across all trials.
        if (cutWeight < bestCut) {
            bestCut = cutWeight;
            bestSide = std::move(currentSide);
        }

        // Zero is optimal for non-negative edge weights.
        if (bestCut == 0) {
            break;
        }
    }

    side = std::move(bestSide);
    return bestCut;
}
