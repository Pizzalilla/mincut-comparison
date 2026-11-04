# AI use log

One entry per use: date, who, tool, what it was used for, and what we did
with the output. This feeds Section 8 of the report.

| Date | Who | Tool | Used for | What we kept or changed |
|---|---|---|---|---|
| 2026-10-25 to 27 | Kartikay | Claude (Claude Code) | Choosing the topic, drafting the proposal email and the week plan, and setting up the repository skeleton: folder layout, Makefile, CMakeLists, `graph.hpp` interface, module stubs, test runner with hand-checked cases, README and templates. Also rechecked the skeleton and verified the test cases' expected answers with a throwaway brute force (not committed). No algorithm, checker, generator or harness code. | |
| 2026-10-27 to 30 | Brian | ChatGPT | Help in implementing the brute-force checker in `brute_force.cpp` and `brute_force.hpp`. Help in implementing Karger's algorithm in `karger.cpp`. | |
| 2026-11-04 | Brian | Gemini | Used to take the benchmark results and find the averages of each instance. | |
| 2026-10-28 to 11-04 | Kartikay | Claude (Claude Code) | Checking that each member's code worked and did what it was meant to do: Brian's Karger rewrite, Ethan's generators and Karger-Stein, and the harness registration of each algorithm. | Found Karger switched off in the harness, `many_cuts` falling back to a cycle when n was not a multiple of 4 and too easy for Karger, Karger-Stein taking about 20 s per run at n = 200 (Ethan then fixed its speed) |
| 2026-10-30 | Kartikay | Claude (Claude Code) | Drafting the benchmark harness, `bench/bench.cpp`. | Reviewed before merging. Fixed time to 99% using the median instead of the mean, added a stop if a randomised result is ever lighter than the true minimum, and added a 0.5 s CPU warm-up after small instances ran about 3× slow. Reverted a Makefile change that would have ignored a member's chosen compiler. |
| 2026-11-02 to 03 | Kartikay | Claude (Claude Code) | Switching Karger on in the harness and changing Ethan's `generateManyCutsGraph` (any n, unit-weight cliques, one weak bridge). Claude also wrote a full set of generators. | Kept the Karger switch and the `many_cuts` change, with a comment in `generators.cpp`. Dropped Claude's generators in favour of Ethan's, which landed the same day. |
| 2026-11-04 | Kartikay | Claude (Claude Code) | Checking that the group's results were computed correctly, and organising the results data into tables and figures: `analysis/plots.py`, `analysis/sw_probe.cpp` and `report/figures/`. | Checked every number against `results/trial.csv`. Fixed wrong values in the first draft (cost per Karger-Stein call, the size below which its recursion removes one vertex per level) and reran the branch-counting probe on the exact benchmark graphs. Redrew the first figures, which were cluttered and reused colours.
| 2026-10-27 | Ethan | Gemini | Generate boilerplate for karger_stein.cpp | |
| 2026-10-28 | Ethan | Gemini | Error checking index and out of bound errors for karger_stein.cpp when mapping recursion | Used a roo_to_compact lookup table |
| 2026-10-28 | Ethan | Gemini | Error checking graph contraction logic in karger_stein.cpp in the buildCompactGraph function | Used std::sort to combine duplicate edges |
| 2026-10-28 | Ethan | Gemini | Error checking for compilation errors in karger_stein.cpp| |
| 2026-10-29 | Ethan | Gemini | Optimising random edge selection in karger_stein.cpp for kargerSteinContract | Used prefixWeights array and used std::upper_bound for edge selection |
| 2026-10-31 | Ethan | Gemini | Generate boilerplate for generator.cpp | |
| 2026-11-01 | Ethan | Gemini | Making therorctical graph specs into generator functions | Made Union-find a common function and changed generateRandomGraph to include the shared UnionFind logic |
| 2026-11-01 | Ethan | Gemini | Error checking for graph logic in generator.cpp | |
| 2026-11-01 | Ethan | Gemini | Generate tests for cycle and barbell graphs in tests.cpp | |
| 2026-11-02 | Ethan | Gemini | Connecting the graph generators and kargerSingleRun to Kartikay's bench.cpp harness | Kept the smoke graph generator for globalwarmup |
| 2026-11-02 | Ethan | Gemini | Analysing the benchmark data to see why Karger-Stein was slower that Karger under 100 verticies | | 
