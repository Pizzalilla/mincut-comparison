// bench.cpp — benchmark harness: times every algorithm on the same instances.
//
// Primary author: Kartikay
//
// For each input family, size n and instance, the harness generates one graph
// and takes Stoer-Wagner's answer as the true minimum cut. It then times
// Stoer-Wagner --reps times, and times --runs single runs of each randomised
// algorithm (seeds 0..runs-1), recording whether each run found the minimum.
//
// Every run becomes one CSV row, so the analysis scripts can compute any
// statistic afterwards. A per-instance summary is printed to stdout: median
// run time, success rate p, runs needed for 99% confidence
// k = ceil(ln 0.01 / ln(1 - p)), and time to 99% = k x mean run time.
//
// Timing: std::chrono::steady_clock around the algorithm call only. Graph
// generation is outside the timed region. Each algorithm's conversion of the
// edge list into its own structure is inside it, because that is part of the
// algorithm's cost. The process warms the CPU up for about 0.5 s before any
// measurement, and one untimed warm-up run precedes each measured batch.
//
// Usage: build/bench [--out PATH] [--sizes 25,50,100] [--families a,b]
//                    [--algorithms a,b] [--instances N] [--runs N]
//                    [--reps N] [--budget SECONDS]

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "algorithms.hpp"
#include "generators.hpp"

namespace {

struct Algorithm {
    std::string name;
    MinCutFn singleRun;  // one run only; nullptr until it exists
    bool randomised;
};

// kargerMinCut is a single contraction run since Brian's rewrite (sorted
// random keys + union-find). kargerSteinMinCut is a single run (one recursion
// tree). The harness does all the repeating.
const std::vector<Algorithm> kAlgorithms = {
    {"stoer-wagner", stoerWagnerMinCut, false},
    {"karger", kargerMinCut, true},
    {"karger-stein", kargerSteinMinCut, true},
};

// Used only by globalWarmUp, to keep the CPU busy before measuring. It is not
// one of the report's input families.
Graph smokeGraph(int n, std::uint64_t seed) {
    std::mt19937_64 rng(seed);
    Graph g{n, {}};
    for (int v = 1; v < n; ++v) g.edges.push_back({v - 1, v, 1});
    for (int u = 0; u < n; ++u)
        for (int v = u + 2; v < n; ++v)
            if (rng() & 1) g.edges.push_back({u, v, 1});
    return g;
}

struct Family {
    std::string name;
    Graph (*make)(int n, std::uint64_t seed);
};

// The input families from generators.hpp.
const std::vector<Family> kFamilies = {
    // Dense random graph (p=0.5, max weight 100)
    {"random_dense", [](int n, std::uint64_t seed) { 
        return generateRandomGraph(n, 0.5, 100, seed); 
    }},
    
    // Sparse random graph (p=0.1, max weight 100)
    {"random_sparse", [](int n, std::uint64_t seed) { 
        return generateRandomGraph(n, 0.1, 100, seed); 
    }},
    
    // Barbell trap graph (max weight 100)
    {"barbell", [](int n, std::uint64_t seed) { 
        return generateBarbellGraph(n, 100, seed); 
    }},
    
    // Cycle graph (weight 1, seed ignored)
    {"cycle", [](int n, std::uint64_t /*seed*/) { 
        return generateCycleGraph(n, 1); 
    }},
    
    // Many cuts graph: a ring of unit-weight cliques of about 7 vertices,
    // bridges of weight 3 and one weak bridge of 2. The minimum cut is 5 (the
    // weak bridge plus any other), and every pair of strong bridges gives a
    // near-minimum cut of 6, as does every vertex of a 7-vertex clique that no
    // bridge touches. Intended for n >= 21.
    {"many_cuts", [](int n, std::uint64_t seed) {
        int k = std::min(n, std::max(3, n / 7));
        return generateManyCutsGraph(n, k, 1, 3, 2, seed);
    }}
};
struct Options {
    std::string out = "results/bench.csv";
    std::vector<int> sizes = {25, 50, 100};
    std::set<std::string> families;    // empty means all
    std::set<std::string> algorithms;  // empty means all
    int instances = 3;
    int runs = 100;
    int reps = 10;
    double budgetSeconds = 30;  // per algorithm, per (family, n, instance)
};

[[noreturn]] void usage(const char* error) {
    if (error) std::fprintf(stderr, "bench: %s\n\n", error);
    std::fprintf(stderr,
        "Usage: bench [--out PATH] [--sizes 25,50,100] [--families a,b]\n"
        "             [--algorithms a,b] [--instances N] [--runs N]\n"
        "             [--reps N] [--budget SECONDS]\n\n"
        "  --runs     single runs per instance for randomised algorithms\n"
        "  --reps     timed runs per instance for Stoer-Wagner\n"
        "  --budget   once an algorithm spends this long on one instance it\n"
        "             stops, and skips the rest of that family\n");
    std::exit(error ? 2 : 0);
}

std::vector<std::string> splitCommas(const std::string& s) {
    std::vector<std::string> parts;
    std::stringstream in(s);
    for (std::string part; std::getline(in, part, ',');)
        if (!part.empty()) parts.push_back(part);
    return parts;
}

int toInt(const std::string& s, const char* flag) {
    try {
        int value = std::stoi(s);
        if (value > 0) return value;
    } catch (const std::exception&) {
    }
    usage((std::string(flag) + " needs a positive integer").c_str());
}

Options parseArgs(int argc, char** argv) {
    Options opt;
    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];
        if (flag == "--help" || flag == "-h") usage(nullptr);
        if (i + 1 >= argc) usage((flag + " needs a value").c_str());
        std::string value = argv[++i];
        if (flag == "--out") {
            opt.out = value;
        } else if (flag == "--sizes") {
            opt.sizes.clear();
            for (const std::string& s : splitCommas(value))
                opt.sizes.push_back(toInt(s, "--sizes"));
        } else if (flag == "--families") {
            for (const std::string& s : splitCommas(value)) opt.families.insert(s);
        } else if (flag == "--algorithms") {
            for (const std::string& s : splitCommas(value)) opt.algorithms.insert(s);
        } else if (flag == "--instances") {
            opt.instances = toInt(value, "--instances");
        } else if (flag == "--runs") {
            opt.runs = toInt(value, "--runs");
        } else if (flag == "--reps") {
            opt.reps = toInt(value, "--reps");
        } else if (flag == "--budget") {
            opt.budgetSeconds = std::atof(value.c_str());
            if (opt.budgetSeconds <= 0) usage("--budget needs a positive number");
        } else {
            usage(("unknown option " + flag).c_str());
        }
    }
    for (const std::string& f : opt.families)
        if (std::none_of(kFamilies.begin(), kFamilies.end(),
                         [&](const Family& fam) { return fam.name == f; }))
            usage(("unknown family " + f).c_str());
    for (const std::string& a : opt.algorithms)
        if (std::none_of(kAlgorithms.begin(), kAlgorithms.end(),
                         [&](const Algorithm& alg) { return alg.name == a; }))
            usage(("unknown algorithm " + a).c_str());
    return opt;
}

std::uint64_t splitmix64(std::uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

// Same (family, n, instance) gives the same graph on every machine, so every
// algorithm and every rerun sees identical inputs. std::hash is not portable,
// so the family name is hashed with FNV-1a.
std::uint64_t instanceSeed(const std::string& family, int n, int instance) {
    std::uint64_t h = 1469598103934665603ULL;
    for (unsigned char c : family) h = (h ^ c) * 1099511628211ULL;
    return splitmix64(splitmix64(h ^ (std::uint64_t)n) ^ (std::uint64_t)instance);
}

struct RunResult {
    double ms;
    long long found;
};

RunResult timeRun(MinCutFn fn, const Graph& g, std::uint64_t seed) {
    std::vector<int> side;
    auto start = std::chrono::steady_clock::now();
    long long found = fn(g, side, seed);
    auto stop = std::chrono::steady_clock::now();
    // Outside the timed region: guard the data against an invalid cut.
    if (cutWeight(g, side) != found) {
        std::fprintf(stderr, "bench: returned %lld but side has weight %lld\n",
                     found, cutWeight(g, side));
        std::exit(1);
    }
    return {std::chrono::duration<double, std::milli>(stop - start).count(), found};
}

double median(std::vector<double> v) {
    std::sort(v.begin(), v.end());
    std::size_t mid = v.size() / 2;
    return v.size() % 2 ? v[mid] : (v[mid - 1] + v[mid]) / 2;
}

double mean(const std::vector<double>& v) {
    double total = 0;
    for (double x : v) total += x;
    return total / (double)v.size();
}

// The first batch in a process measured 10-40% slower than the same graph
// straight afterwards (CPU ramping up), which a per-batch warm-up does not
// cover. Keep the CPU busy for about half a second before measuring anything.
void globalWarmUp() {
    const Graph g = smokeGraph(60, 1);
    std::vector<int> side;
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < std::chrono::milliseconds(500))
        stoerWagnerMinCut(g, side, 0);
}

// Runs needed to find the minimum with probability 0.99, given per-run
// success rate p. Returns -1 when p == 0 (never succeeded: unbounded).
long long runsFor99(double p) {
    if (p <= 0) return -1;
    if (p >= 1) return 1;
    return (long long)std::ceil(std::log(0.01) / std::log(1 - p));
}

}  // namespace

int main(int argc, char** argv) {
    const Options opt = parseArgs(argc, argv);

    std::filesystem::path outPath(opt.out);
    if (outPath.has_parent_path())
        std::filesystem::create_directories(outPath.parent_path());
    std::ofstream csv(outPath);
    if (!csv) {
        std::fprintf(stderr, "bench: cannot write %s\n", opt.out.c_str());
        return 1;
    }
    csv << "family,n,m,instance,algorithm,run,seed,time_ms,found,truth,success\n";

    std::printf("compiler: %s\ncommand:", __VERSION__);
    for (int i = 0; i < argc; ++i) std::printf(" %s", argv[i]);
    std::printf("\nwriting:  %s\n\n", opt.out.c_str());
    globalWarmUp();
    std::printf("%-16s %5s %7s %4s  %-13s %5s %11s %7s %10s %13s\n", "family", "n",
                "m", "inst", "algorithm", "runs", "median_ms", "p", "runs_99",
                "time_99_ms");

    for (const Family& family : kFamilies) {
        if (!opt.families.empty() && !opt.families.count(family.name)) continue;
        std::set<std::string> overBudget;  // skip the rest of this family

        for (int n : opt.sizes) {
            for (int inst = 0; inst < opt.instances; ++inst) {
                const Graph g = family.make(n, instanceSeed(family.name, n, inst));
                std::vector<int> side;
                const long long truth = stoerWagnerMinCut(g, side, 0);

                for (const Algorithm& alg : kAlgorithms) {
                    if (!alg.singleRun || overBudget.count(alg.name)) continue;
                    if (!opt.algorithms.empty() && !opt.algorithms.count(alg.name))
                        continue;

                    const int count = alg.randomised ? opt.runs : opt.reps;
                    timeRun(alg.singleRun, g, (std::uint64_t)count);  // warm-up

                    std::vector<double> times;
                    int hits = 0;
                    double spentMs = 0;
                    for (int r = 0; r < count; ++r) {
                        const RunResult res = timeRun(alg.singleRun, g, (std::uint64_t)r);
                        // A valid cut lighter than Stoer-Wagner's means the
                        // ground truth itself is wrong: stop rather than
                        // record it as a failed run.
                        if (res.found < truth) {
                            std::fprintf(stderr,
                                         "bench: %s found %lld < Stoer-Wagner's %lld "
                                         "(%s, n=%d, instance %d)\n",
                                         alg.name.c_str(), res.found, truth,
                                         family.name.c_str(), n, inst);
                            return 1;
                        }
                        const bool success = res.found == truth;
                        hits += success;
                        times.push_back(res.ms);
                        spentMs += res.ms;
                        csv << family.name << ',' << n << ',' << g.edges.size() << ','
                            << inst << ',' << alg.name << ',' << r << ',' << r << ','
                            << res.ms << ',' << res.found << ',' << truth << ','
                            << success << '\n';
                        if (spentMs > opt.budgetSeconds * 1000) {
                            overBudget.insert(alg.name);
                            break;
                        }
                    }
                    csv.flush();  // keep partial data if a later run crashes

                    const double med = median(times);
                    const double p = (double)hits / (double)times.size();
                    const long long k = runsFor99(p);
                    char k99[32], t99[32];
                    if (k < 0) {
                        std::snprintf(k99, sizeof k99, "never");
                        std::snprintf(t99, sizeof t99, "-");
                    } else {
                        std::snprintf(k99, sizeof k99, "%lld", k);
                        // Expected total time of k runs is k x the mean run
                        // time, not the median.
                        std::snprintf(t99, sizeof t99, "%.3f", (double)k * mean(times));
                    }
                    std::printf("%-16s %5d %7zu %4d  %-13s %5zu %11.4f %7.3f %10s %13s%s\n",
                                family.name.c_str(), n, g.edges.size(), inst,
                                alg.name.c_str(), times.size(), med, p, k99, t99,
                                overBudget.count(alg.name) ? "  (over budget: rest of family skipped)" : "");
                    std::fflush(stdout);
                }
            }
        }
    }
    return 0;
}
