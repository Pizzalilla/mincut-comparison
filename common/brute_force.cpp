// brute_force.cpp — exact minimum cut by trying every bipartition (small n).
//
// Primary author: Brian

#include "brute_force.hpp"
#include <cstdint>
#include <stdexcept>

long long bruteForceMinCut(const Graph& g, std::vector<int>& side,
                           std::uint64_t seed) {
    (void)g, (void)side, (void)seed;
    // throw std::logic_error("bruteForceMinCut: not implemented yet");
    if (g.n < 2) {
        throw std::invalid_argument(
            "bruteForceMinCut: graph must have at least 2 vertices");
    }

    if (g.n > 16) {
        throw std::invalid_argument(
            "bruteForceMinCut: supports at most 16 vertices");
    }

    // Validate edge endpoints.
    for (const Edge& edge : g.edges) {
        if (edge.u < 0 || edge.u >= g.n ||
            edge.v < 0 || edge.v >= g.n) {
            throw std::invalid_argument(
                "bruteForceMinCut: edge endpoint is out of range");
        }
    }

    // Fix vertex 0 to side 0 to avoid checking each cut twice.
    // Bit (v - 1) of mask determines the side of vertex v.
    // Mask 0 is excluded because it produces an empty side 1.
    const std::uint64_t subsetCount = std::uint64_t{1} << (g.n - 1);
    long long bestWeight = 0;
    std::uint64_t bestMask = 1;
    bool haveBest = false;
    const auto isOnSide1 = [](std::uint64_t mask, int vertex) -> bool {
        return vertex != 0 &&
            (mask & (std::uint64_t{1} << (vertex - 1))) != 0;
    };

    // Enumerate every non-trivial cut.
    for (std::uint64_t mask = 1; mask < subsetCount; ++mask) {
        long long cutWeight = 0;
        // Sum the weights of edges crossing this cut.
        for (const Edge& edge : g.edges) {
            if (isOnSide1(mask, edge.u) !=
                isOnSide1(mask, edge.v)) {
                cutWeight += edge.w;
            }
        }
        // Keep the smallest cut found so far.
        if (!haveBest || cutWeight < bestWeight) {
            bestWeight = cutWeight;
            bestMask = mask;
            haveBest = true;
        }
    }
    // Reconstruct the partition for the minimum cut.
    side.assign(g.n, 0);
    for (int v = 1; v < g.n; ++v) {
        side[v] =
            (bestMask & (std::uint64_t{1} << (v - 1))) != 0
                ? 1
                : 0;
    }
    return bestWeight;
}
