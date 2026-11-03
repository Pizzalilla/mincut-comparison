// generators.cpp — input families for the experiments.
//
// Primary author: Ethan

#include "generators.hpp"
#include <random>
#include <vector>
#include <algorithm>
#include <numeric>

#include "union_find.hpp"

Graph generateRandomGraph(int n, double p, long long maxWeight, std::uint64_t seed) 
{
    Graph g;
    g.n = n;
    if (n < 2) 
    {
        return g;
    }

    std::mt19937_64 rng(seed);
    std::uniform_real_distribution<double> dist_p(0.0, 1.0);
    std::uniform_int_distribution<long long> dist_w(1, maxWeight);

    UnionFind uf(n);
    int components = n;

    for (int u = 0; u < n; ++u) 
    {
        for (int v = u + 1; v < n; ++v) 
        {
            if (dist_p(rng) <= p) 
            {

                g.edges.push_back({u, v, dist_w(rng)});
                if (uf.unite(u, v)) 
                {
                    components--;
                }
            }
        }
    }

    if (components > 1) 
    {
        std::vector<int> nodes(n);
        std::iota(nodes.begin(), nodes.end(), 0);
        std::shuffle(nodes.begin(), nodes.end(), rng);
        
        for (int i = 1; i < n; ++i) 
        {
            if (uf.unite(nodes[i-1], nodes[i])) 
            {
                g.edges.push_back({nodes[i-1], nodes[i], dist_w(rng)});

            }
        }
    }

    return g;
}


Graph generateBarbellGraph(int n, long long maxWeight, std::uint64_t seed) 
{
    Graph g;
    g.n = n;
    if (n < 2) 
    {
        return g;
    }

    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<long long> dist_w(1, maxWeight);

    int half = n / 2;

    // left clique
    for (int u = 0; u < half; ++u) 
    {
        for (int v = u + 1; v < half; ++v) 
        {
            g.edges.push_back({u, v, dist_w(rng)});
        }
    }

    // right clique
    for (int u = half; u < n; ++u) 
    {
        for (int v = u + 1; v < n; ++v) 
        {
            g.edges.push_back({u, v, dist_w(rng)});
        }
    }

    g.edges.push_back({half - 1, half, 1});

    return g;
}

Graph generateCycleGraph(int n, long long weight) 
{
    Graph g;
    g.n = n;
    
    if (n < 2)
    {
        return g;
    }

    for (int i = 0; i < n; ++i) 
    {
        g.edges.push_back({i, (i + 1) % n, weight});
    }

    return g;
}


// Changed by Claude (AI) at Kartikay's request on 2026-11-03: any n now works
// (the first n % k clusters get one extra vertex), and the bridge from the
// last cluster back to the first is the weak one, with weight weakBridgeWeight.
Graph generateManyCutsGraph(int n, int k, long long maxWeight, long long bridgeWeight, long long weakBridgeWeight, std::uint64_t seed)
{
    Graph g;
    g.n = n;
    if (k < 2 || n < k)
    {
        return g;
    }

    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<long long> dist_w(1, maxWeight);

    // cluster c holds vertices start[c] .. start[c + 1] - 1
    std::vector<int> start(k + 1, 0);
    for (int c = 0; c < k; ++c)
    {
        start[c + 1] = start[c] + n / k + (c < n % k ? 1 : 0);
    }

    for (int c = 0; c < k; ++c)
    {
        for (int u = start[c]; u < start[c + 1]; ++u)
        {
            for (int v = u + 1; v < start[c + 1]; ++v)
            {
                g.edges.push_back({u, v, dist_w(rng)});
            }
        }
    }

    // first vertex of cluster c to last vertex of cluster c + 1
    for (int c = 0; c < k; ++c)
    {
        int u = start[c];
        int vCluster = (c + 1) % k;
        int v = start[vCluster + 1] - 1;

        g.edges.push_back({u, v, c == k - 1 ? weakBridgeWeight : bridgeWeight});
    }

    return g;
}