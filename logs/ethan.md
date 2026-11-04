# Bug and design log: Ethan

One entry per bug or decision, written when it happens: date, what went
wrong or what the choice was, how it was resolved, and why.

| Date | What went wrong | How was it resolved and why |
|---|---|---|
| 2026-10-28 | Passing the original edge list down the recursion tree caused child calls prosessing edges, which broke the theritcal bound | Implemented a dedicated buildCompactGraph function |
| 2026-10-29 | Vertex merges created duplicate parallel edges | Sorted the active_edge array and linearly merged the weights of adjacent duplicate edges |
| 2026-10-29 | Returning the final cut partition caused state corruption | Made root_to_compact lookup table to map Union-Find parent ID |
| 2026-10-29 | Selecting an edge prortional to its weight creating a CPU bottleneck  | Made a prefixWeights array and used std::upper_bound  to find the target edge |
| 2026-10-31 | Karger and Karger-Stein and generators need disjoint set logic | Created common union find file
| 2026-11-03 | Benchmarks showed standard Karger beating Karger-Stein in wall-clock time | Left the implementation as is to show  theory vs. hardware for the final report |