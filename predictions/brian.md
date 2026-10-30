# Predictions: Brian

Written on 2026-10-28 and 2026-10-29 before running any benchmark. Commit this file before experiments
start and do not edit it afterwards; later thoughts go in the report.

## Which algorithm wins on which input family, and why
**Large Dense Graphs:** I think the Karger-Stein algorithm will be the fastest followed by Karger and Stoer-Wagner. This is because Stoer-Wagner is a deterministic algorithm and I think it will struggle on this input family in comparisons to the other 2 randomised algorithms. I think Karger-Stein will be faster than Karger because it uses recursive branching rather than random contradictions and many repeats.
**Smaller Spares Graphs** I think the the speed between Karger-Stein and Stoer-Wagner will be very similar but in this case, Stoer-Wagner is deterministic and will have a higher success rate so I think overall the winner will be Stoer-Wagner followed by Karger-Stein and Karger in dead last. Because the graph is smaller and less dense, the deterministic approach shouldn't make that much of a difference interms of speed.
## Where the crossovers fall (graph size, density)
I think there will be a crossover when the graph size and density reaches a certain point. At this point I think the graph size and density will be very large, somewhere around a thousand vertices and the Karger-Stein algorithm will run away as the faster/winning algorithm, but before that point, I think the race will be neck and neck.
## Success rates of the randomised algorithms compared with their bounds
I think the Karger-Stein will beat the Karger algorithm interms of success rate because it does not use random contradiction, it uses recursive branching which I think will be more consistent.
## Anything else I expect to see
I expect to see Stoer-Wagner have a higher success rate than both randomised algorithms. I also expect to see the benchmarks interms of efficiency between the three algorithms be miniscule when there is less than a thousand vertices (smaller graphs). I think the bigger differences will only be seen on large graphs.