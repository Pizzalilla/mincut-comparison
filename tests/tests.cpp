// tests.cpp — correctness tests for every min-cut implementation.
//
// Shared by the whole group. Each case is a small hand-checked graph with a
// known minimum cut. Every implementation must return a valid cut (both
// sides non-empty, returned weight matches side). Deterministic ones must
// return exactly the minimum; randomised ones must never return less, and we
// report how many of 20 seeds found the minimum.
//
// An implementation that still throws "not implemented" is reported as
// SKIP, not FAIL, so the suite stays green while modules are being written.

#include <cstdio>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

#include "algorithms.hpp"
#include "brute_force.hpp"
#include "generators.hpp"

namespace {

struct Case {
    std::string name;
    Graph g;
    long long minCut;
};

struct Impl {
    std::string name;
    MinCutFn fn;
    bool randomised;
};

Graph makeGraph(int n, std::vector<Edge> edges) {
    return Graph{n, std::move(edges)};
}

std::vector<Case> cases() {
    return {
        {"single edge", makeGraph(2, {{0, 1, 5}}), 5},
        {"weighted path", makeGraph(4, {{0, 1, 3}, {1, 2, 1}, {2, 3, 4}}), 1},
        {"cycle of 5",
         makeGraph(5, {{0, 1, 1}, {1, 2, 1}, {2, 3, 1}, {3, 4, 1}, {4, 0, 1}}),
         2},
        {"K4",
         makeGraph(4, {{0, 1, 1}, {0, 2, 1}, {0, 3, 1},
                       {1, 2, 1}, {1, 3, 1}, {2, 3, 1}}),
         3},
        {"two triangles + bridge",
         makeGraph(6, {{0, 1, 1}, {1, 2, 1}, {0, 2, 1},
                       {3, 4, 1}, {4, 5, 1}, {3, 5, 1}, {2, 3, 1}}),
         1},
        {"parallel edges",
         makeGraph(3, {{0, 1, 1}, {0, 1, 1}, {0, 1, 1},
                       {1, 2, 1}, {1, 2, 1}, {0, 2, 1}}),
         3},
        // The worked example from Stoer & Wagner (1997), relabelled 0..7.
        {"Stoer-Wagner paper",
         makeGraph(8, {{0, 1, 2}, {0, 4, 3}, {1, 2, 3}, {1, 4, 2},
                       {1, 5, 2}, {2, 3, 4}, {2, 6, 2}, {3, 6, 2},
                       {3, 7, 2}, {4, 5, 3}, {5, 6, 1}, {6, 7, 3}}),
         4},
         {"cycle", 
         generateCycleGraph(6, 1), 
         2},
         
        {"barbell", 
         generateBarbellGraph(6, 10, 42), // 42 is the random seed fn
         1},
    };
}

// print graph REMOVE ON SUBMISSION
// void printGraph(const std::string& name, const Graph& g) {
//     std::cout << "--- " << name << " (V=" << g.n << ", E=" << g.edges.size() << ") ---\n";
//     for (const auto& e : g.edges) {
//         std::cout << e.u << " -- " << e.v << " [weight: " << e.w << "]\n";
//     }
//     std::cout << "\n";
// }

// Empty string if side is a valid cut of weight `claimed`, else the reason.
std::string checkCut(const Graph& g, const std::vector<int>& side,
                     long long claimed) {
    if ((int)side.size() != g.n) return "side has wrong size";
    int ones = 0;
    for (int s : side) {
        if (s != 0 && s != 1) return "side contains a value other than 0/1";
        ones += s;
    }
    if (ones == 0 || ones == g.n) return "one side of the cut is empty";
    long long actual = cutWeight(g, side);
    if (actual != claimed)
        return "returned " + std::to_string(claimed) + " but side has weight " +
               std::to_string(actual);
    return "";
}

}  // namespace

int main() {
    const std::vector<Impl> impls = {
        {"brute force", bruteForceMinCut, false},
        {"Stoer-Wagner", stoerWagnerMinCut, false},
        {"Karger", kargerMinCut, true},
        {"Karger-Stein", kargerSteinMinCut, true},
    };
    const int seeds = 20;
    int failures = 0;

    for (const Impl& impl : impls) {
        std::printf("== %s\n", impl.name.c_str());
        for (const Case& c : cases()) {
            int hits = 0;
            std::string error;
            bool skipped = false;
            for (int s = 0; s < (impl.randomised ? seeds : 1) && error.empty(); ++s) {
                std::vector<int> side;
                long long got;
                try {
                    got = impl.fn(c.g, side, (std::uint64_t)s);
                } catch (const std::logic_error& e) {
                    if (std::string(e.what()).find("not implemented") != std::string::npos) {
                        skipped = true;
                        break;
                    }
                    error = std::string("threw: ") + e.what();
                    break;
                } catch (const std::exception& e) {
                    error = std::string("threw: ") + e.what();
                    break;
                } catch (...) {
                    error = "threw a non-standard exception";
                    break;
                }
                error = checkCut(c.g, side, got);
                if (error.empty() && got < c.minCut)
                    error = "returned " + std::to_string(got) +
                            ", below the true minimum " + std::to_string(c.minCut);
                if (error.empty() && !impl.randomised && got != c.minCut)
                    error = "returned " + std::to_string(got) + ", expected " +
                            std::to_string(c.minCut);
                if (error.empty() && got == c.minCut) ++hits;
            }

            if (skipped) {
                std::printf("  SKIP  %-24s (not implemented)\n", c.name.c_str());
                break;  // the rest of this implementation's cases would skip too
            }
            if (!error.empty()) {
                ++failures;
                std::printf("  FAIL  %-24s %s\n", c.name.c_str(), error.c_str());
            } else if (impl.randomised) {
                std::printf("  PASS  %-24s found minimum in %d/%d seeds\n",
                            c.name.c_str(), hits, seeds);
            } else {
                std::printf("  PASS  %s\n", c.name.c_str());
            }
        }
    }

    // for testing graphs
    // printGraph("Cycle", generateCycleGraph(6, 1));
    // printGraph("Barbell", generateBarbellGraph(6, 10, 42));

    std::printf("\n%s (%d failure%s)\n", failures ? "FAILED" : "OK", failures,
                failures == 1 ? "" : "s");
    return failures ? 1 : 0;
}
