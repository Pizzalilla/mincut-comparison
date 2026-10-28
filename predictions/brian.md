# Predictions: Brian

Written before running any benchmark. Commit this file before experiments
start and do not edit it afterwards; later thoughts go in the report.

## Which algorithm wins on which input family, and why
I think the Karger-Stein algorithm will be the fastest out of the 3 on a large and dense unweighted graphs. I think it will be the fastest here because Stoer-Wagner is deterministic and will struggle on large graphs in comparison to the Karger-Stein and Karger algorithms which are randomised and do not consider every vertices like the deterministic way. Between Karger-Stein and Karger, I am not fully sure but I am guessing Karger-Stein would win because it doesn't use random contradiction, I think this is what separates the 2 algorithms interms of efficiency.
## Where the crossovers fall (graph size, density)
I think there will be a crossover when the graph size and density reaches a certain point or threshold. At this point I think the graph size and density will be very large, somewhere around a couple thousand vertices.
## Success rates of the randomised algorithms compared with their bounds
I think the Karger-Stein will beat the Karger algorithm interms of success rate because it does not use random contradiction, it uses recursive branching which I think will be more consistent.
## Anything else I expect to see
I expect to see Stoer-Wagner have a higher success rate than both randomised algorithms. I also expect to see the benchmarks interms of efficiency between the three algorithms be miniscule when there is less than a thousand vertices (smaller graphs). I think the bigger differences will only be seen on large graphs.