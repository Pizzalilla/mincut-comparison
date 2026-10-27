// stoer_wagner.cpp — Stoer-Wagner deterministic minimum cut.
//
// Primary author: Kartikay

#include <limits>
#include <numeric>
#include <stdexcept>

#include "algorithms.hpp"

long long stoerWagnerMinCut(const Graph& g, std::vector<int>& side,
                            std::uint64_t seed) {
    (void)seed;  // deterministic
    const int n = g.n;

    // Step 1: set up.
    // mat[u][v] is the total weight between (merged) vertices u and v.
    // += so parallel edges add up.
    std::vector<std::vector<long long>> mat(n, std::vector<long long>(n, 0));
    for (const Edge& e : g.edges) {
        mat[e.u][e.v] += e.w;
        mat[e.v][e.u] += e.w;
    }

    // groups[v] lists the original vertices merged into v.
    std::vector<std::vector<int>> groups(n);
    for (int v = 0; v < n; ++v) groups[v] = {v};

    // active lists the vertices not yet merged away.
    std::vector<int> active(n);
    std::iota(active.begin(), active.end(), 0);

    long long best = std::numeric_limits<long long>::max();
    std::vector<int> bestGroup;

    // Step 2: one phase.
    // w[v] is v's total weight to the vertices already in A.
    std::vector<long long> w(n, 0);
    std::vector<bool> inA(n, false);
    for (int v : active) {
        w[v] = 0;
        inA[v] = false;
    }
    int prev = -1, last = -1;
    for (std::size_t step = 0; step < active.size(); ++step) {
        // Pick the active vertex outside A that is most tightly connected to A.
        int next = -1;
        for (int v : active)
            if (!inA[v] && (next == -1 || w[v] > w[next])) next = v;

        inA[next] = true;
        prev = last;
        last = next;
        for (int u : active)
            if (!inA[u]) w[u] += mat[next][u];
    }
    // last is in A now, so the update loop above never touched w[last]:
    // it is still last's connection to everything added before it.
    const long long cutOfPhase = w[last];

    // TODO Steps 3-5: record best cut, merge, repeat and fill side.
    (void)best, (void)bestGroup, (void)side, (void)prev, (void)cutOfPhase;
    throw std::logic_error("stoerWagnerMinCut: not implemented yet");
}
