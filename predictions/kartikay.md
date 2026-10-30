# Predictions: Kartikay

Written on 2026-10-28, before running any benchmark. Commit this file before
experiments start and do not edit it afterwards; later thoughts go in the
report.

Notation: n vertices, m edges. "Time to 99%" means a randomised algorithm is
repeated until it finds the minimum cut with probability 0.99.

## Which algorithm wins on which input family, and why

**P1. Large dense graphs: Stoer-Wagner first, then Karger-Stein, with Karger
far behind.** Karger succeeds with probability at least 2/(n(n−1)), so
reaching 99% takes about ln(100)·n²/2 ≈ 2.3n² runs, each costing O(n²) on a
dense graph: about O(n⁴) in total. Karger-Stein is O(n² log n) per run and
needs only O(log n) repeats, so about O(n² log² n). That is asymptotically
better than Stoer-Wagner's O(n³), but its recursion carries large constants.
At the sizes we can realistically test, I expect Stoer-Wagner's tight, simple
loops to still win.

**P2. Large sparse graphs: Stoer-Wagner again, with Karger last.** Karger's
repetition count is fixed by the bound, not by the graph, so sparsity doesn't
help it much. Even at O(m) per run with union-find, 2.3n² runs on a graph with
m ≈ n still cost about n³. Karger-Stein is usually matrix-based, so sparsity
doesn't help it either. My Stoer-Wagner is matrix-based too, so it doesn't
gain from sparsity, but I expect it to keep its lead.

**P3. On cycles and planted-cut graphs, Karger beats Stoer-Wagner at every
size we test, when its repetition count is set from the measured success
rate.** This covers only those two families. P1 and P2 cover random dense
and sparse graphs, where I expect Karger to lose. On cycles (P6) and planted
cuts, a single Karger run finds the minimum nearly every time, so only a
handful of runs reach 99%, which is far cheaper than Stoer-Wagner's O(n³). On
random graphs and graphs with many near-minimum cuts, its success rate is
too low and the repetitions make it lose.

## Where the crossovers fall (graph size, density)

**P4. Karger-Stein overtakes Stoer-Wagner only above n ≈ 1000.** If our test
range tops out below that, it never overtakes. The asymptotic gap is only
about n/log²n, and Karger-Stein's two-branch recursion plus about 4.6·log n
repeats eat that gap until n is in the low thousands. A rough estimate at
n = 1000 has the two algorithms within a small constant factor of each other.

**P7. Doubling n multiplies Stoer-Wagner's time by about 8, and graph
structure makes essentially no difference.** My implementation uses an
adjacency matrix, so it is O(n³). Every phase scans full rows whether the
entries are zero or not, so a cycle and a dense random graph with the same n
take about the same time. A structure effect would only appear with an
adjacency-list and heap version, where sparse graphs get noticeably faster.

## Success rates of the randomised algorithms compared with their bounds

**P5. On random graphs, Karger's real success rate is 100× or more above
2/(n(n−1)).** The bound is tight only when every vertex has degree about k,
the min-cut value, throughout the contraction. In a random graph the min cut
is usually just the minimum-degree vertex. Once supervertices form, their
degrees far exceed k, so the edge count stays much larger than kn/2 and the
chance of contracting a min-cut edge drops sharply. Several vertices often tie
for minimum degree, which adds extra min cuts that count as successes. At
n = 100 the bound is about 0.02%; I expect the measured rate to be several
percent or more.

**P6. Hardest family for the randomised algorithms: many near-minimum cuts.
Easiest: the cycle.** When many cuts are only slightly heavier than the true
minimum, Karger tends to land on one of those instead. The cycle is the
textbook case where the bound is tight, but only for finding one specific min
cut. Every run on a cycle ends with two arcs joined by 2 edges, which is
always a minimum cut, so the success rate for finding the min-cut value should
be 100%. A planted cut should also be easy, because its few crossing edges are
rarely picked among the many internal ones.

## Anything else I expect to see

**P8. Inside Stoer-Wagner: picking the next vertex about 35–50% of the time,
updating w about 50–65%, merging under 2%.** In each phase with n′ active
vertices, both picking the maximum and updating w touch about n′ entries per
step, so about n′² per phase each. That makes them roughly equal in theory,
but updating w should cost a bit more in practice because it reads a matrix
row as well as writing w, while the pick is a read-only scan of w. Merging
happens once per phase at O(n) cost, so its share shrinks like 1/n. My code
keeps the pick and the update as separate loops, so they can be timed
separately.
