# Predictions: Ethan

Written before running any benchmark. Commit this file before experiments
start and do not edit it afterwards; later thoughts go in the report.

## Which algorithm wins on which input family, and why
**In Sparce Graphs**
Stoer-Wagner > Karger-Stein >= Karger
Stoer-Wagner should be the fastests in sparce graphs, followed by Karger-Stein and Karger. Stoer-Wagner is time complexity is tied to the amound of edges, so in spares graphs with less edges, it should beat the other two. This is because it is a determinisic algorithm and the other two are randomised, which means that Karger and Karger-Stein have higher memoery costs, and the advantage of being asymptotic wouldn't factor as much.

**In Dense Graphs**
Karger-Stein > Karger > Stoer-Wagner
I think Karger-Stein will be the fastest in dense graphs because it shrinks the edges space more than the other two every step. This means that it should be relativly uneffected by the density of a graph compared to Stoer-Wagner, which means it should over take it despite the higher memory costs. It should be faster that Karger as well, because it is recursive. 


## Where the crossovers fall (graph size, density)
**Size**
For small graphs I expect Soter-Wagner to beat Karger/Karger-Stein due to Karger-Stein being recurisive by having moving vectors, which is heavy. I think the crossover will be at around 500 verticies, where the scaling of Karger-Stein will beat its recursive overhead. 

**Density**
For denisty, I expect the crossover to take place when the density increase. This is because Kerger-Steins density should remain relatiely flat, and Stoer-Wagner should scale with the density.


## Success rates of the randomised algorithms compared with their bounds
The success bound for Karger is >= 2/n(n - 1) and the success bound for Karger-Stein is >= 1/log(n). The randomly generated graphs the success rates should be higher than these for Karger and Karger-Stein, due to these being worst-case scenarios and the algortims being random. This means that there is a chance that they will find the minimum cut by chance. The hardest should be cycle graphs, due there only being one mininum cut, which means there is a lesser chance of finding it. 


## Anything else I expect to see
- Stoer-Wagner will have the lowest variance, when looking at the same graph, of the three because it is deterministic
- On larger graphs, Karger and Karger-Stein might run into bottleneck problems on the CPU due to the dynamic memory allocation