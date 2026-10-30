# Minimum Cut: Stoer-Wagner vs Karger vs Karger-Stein

Programming Assignment 2: group algorithm comparison, Theme 2 (deterministic
vs randomised).

**Problem.** Given a connected, undirected graph with non-negative integer
edge weights, split the vertices into two non-empty sets so that the total
weight of edges crossing between them is as small as possible.

**Question.** Which algorithm reaches a fixed confidence (such as 99%)
fastest on each input family: Karger's many cheap repetitions, Karger-Stein's
fewer, more expensive ones, or one exact Stoer-Wagner run? And how far are
the randomised algorithms' observed success rates from their theoretical
bounds?

## Algorithms and authors

| Module | Algorithm | Type | Primary author |
|---|---|---|---|
| [`stoer_wagner/`](stoer_wagner/) | Stoer-Wagner | Deterministic | Kartikay |
| [`karger/`](karger/) | Karger's contraction, repeated | Randomised (Monte Carlo) | Brian |
| [`karger_stein/`](karger_stein/) | Karger-Stein recursive contraction | Randomised (Monte Carlo) | Ethan |

Shared tooling:

| File | Purpose | Primary author |
|---|---|---|
| [`include/graph.hpp`](include/graph.hpp) | Graph type and `MinCutFn` interface | Group (initial version AI-assisted) |
| [`common/brute_force.cpp`](common/brute_force.cpp) | Exact min cut by enumeration (small n) | Brian |
| [`common/generators.cpp`](common/generators.cpp) | Input families | Ethan |
| [`bench/bench.cpp`](bench/bench.cpp) | Timing harness | Kartikay |

The repository skeleton (layout, build files, interface, test runner,
templates) was set up with AI assistance; see [`logs/ai-use.md`](logs/ai-use.md).
| [`tests/tests.cpp`](tests/tests.cpp) | Correctness tests | Group (initial version AI-assisted) |

## Building and running

Needs a C++20 compiler and `make` (both come with Xcode Command Line Tools on
macOS, or `build-essential` on Linux). Nothing else to install.

```bash
make test
```

builds the tests with AddressSanitizer and UBSan and runs them against every
implementation. Modules that are not written yet show as `SKIP`.

```bash
make bench
```

builds the benchmark harness with `-O2` into `build/bench`. Run it with, for
example:

```bash
./build/bench --sizes 25,50,100 --instances 3 --runs 100 --reps 10 --out results/trial.csv
```

For each family, size and instance it generates one graph, takes
Stoer-Wagner's answer as the true minimum cut, times Stoer-Wagner `--reps`
times and each randomised algorithm for `--runs` single runs, and writes one
CSV row per run:

```text
family,n,m,instance,algorithm,run,seed,time_ms,found,truth,success
```

It also prints a summary per instance: median run time, success rate p, runs
needed for 99% confidence ⌈ln 0.01 / ln(1 − p)⌉, and time to 99%. Use
`--families`, `--algorithms` and `--budget` (seconds per algorithm per
instance; past it, that algorithm skips the rest of the family) to narrow a
run. `./build/bench --help` lists every option.

Compiler flags used for every reported measurement: `-std=c++20 -O2 -DNDEBUG`
(see `BENCH_FLAGS` in the [Makefile](Makefile)). If they change, update this
line.

## Interface

Every algorithm implements the same `MinCutFn` signature (see
[`include/graph.hpp`](include/graph.hpp)) under its own name:
`stoerWagnerMinCut`, `kargerMinCut`, `kargerSteinMinCut` (declared in
[`include/algorithms.hpp`](include/algorithms.hpp)), and `bruteForceMinCut`
for the checker.

```cpp
long long stoerWagnerMinCut(const Graph& g, std::vector<int>& side,
                            std::uint64_t seed);
```

It returns the cut weight and fills `side[v]` with 0 or 1. Deterministic
algorithms ignore `seed`. Each algorithm converts the edge list into whatever
internal structure it needs.

## Layout

```text
include/        shared types and the algorithm declarations
stoer_wagner/   Kartikay
karger/         Brian
karger_stein/   Ethan
common/         brute-force checker, input generators
tests/          correctness tests (make test)
bench/          timing harness (make bench)
scripts/        experiment and plotting scripts
results/        raw benchmark output (CSV)
predictions/    each member's predictions, committed before any benchmark
logs/           bug/decision logs per member, AI-use log
report/         report drafts
```

## Group rules

1. **Predictions before benchmarks.** Everyone commits their file in
   `predictions/` before anyone runs an experiment. The git history is our
   evidence for the report's predictions section.
2. **Same instances for everyone.** Generators take explicit seeds, and every
   algorithm runs on exactly the same graphs.
3. **Keep your log.** Write bugs and design decisions in `logs/<name>.md` as
   they happen; the individual reflections draw on them.
4. **Record AI use** in `logs/ai-use.md` as you go.
5. **Merge daily** and keep `make test` passing on `main`.

## Still to agree

- [x] Whether each algorithm's internal graph conversion counts towards its
      measured time: **yes**, it is part of the algorithm's cost. Graph
      generation is not timed.
- [x] Timing method: `std::chrono::steady_clock` around the call only, one
      untimed warm-up per batch, report the median
- [x] CSV format for `results/`: see "Building and running"
- [ ] Sizes and parameters for each input family
- [ ] `kargerSingleRun` (Brian) and the real families (Ethan) registered in
      `bench/bench.cpp`, replacing the `smoke` placeholder
