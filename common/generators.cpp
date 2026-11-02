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


Graph generateManyCutsGraph(int n, int k, long long maxWeight, long long bridgeWeight, std::uint64_t seed) 
{
    Graph g;
    g.n = n;
    if (n < 2 || k < 2 || n % k != 0)
    {
        return g;
    } 

    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<long long> dist_w(1, maxWeight);

    int clusterSize = n / k;


    for (int c = 0; c < k; ++c) 
    {
        int start = c * clusterSize;
        int end = start + clusterSize;

        for (int u = start; u < end; ++u) 
        {
            for (int v = u + 1; v < end; ++v) 
            {
                g.edges.push_back({u, v, dist_w(rng)});

            }
        }
    }


    for (int c = 0; c < k; ++c) 
    {
        int u = c * clusterSize; 
        int vCluster = (c + 1) % k;

        int v = vCluster * clusterSize + (clusterSize - 1);
        
        g.edges.push_back({u, v, bridgeWeight});
    }

    return g;
}