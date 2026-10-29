// karger_stein.cpp — Karger-Stein recursive contraction.
//
// Primary author: Ethan

#include <vector>
#include <random>
#include <cmath>
#include <utility>
#include <algorithm>
#include <cstdint>

#include "algorithms.hpp"

struct UnionFind
{
    std::vector<int> parent;
    std::vector<int> rank;

    int numSets = 0;
    
    UnionFind(int n) : parent(n), rank(n, 0), numSets(n)
    {
        for (int i = 0; i < n; ++i) 
        {
            parent[i] = i;
        }
    }
    
    int find(int i) 
    {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]); 
    }
    
    bool unite(int i, int j) 
    {
        int root_i = find(i);
        int root_j = find(j);

        if (root_i == root_j) 
        {
            return false;
        }

        if (rank[root_i] < rank[root_j]) 
        {
            parent[root_i] = root_j;
        } 
        
        else if (rank[root_i] > rank[root_j]) 
        {
            parent[root_j] = root_i;
        }
        
        else 
        {
            parent[root_j] = root_i;
            rank[root_i]++;
        }

        numSets--;

        return true;
    }
};

struct CutResult 
{
    long long weight = 0;
    std::vector<int> side;
};



void kargerSteinContract(UnionFind& uf, const std::vector<Edge>& edges, int target_nodes, std::mt19937_64& rng) 
{
    while (uf.numSets > target_nodes) 
    {
        long long totalWeight = 0;
        std::vector<long long> prefixWeights;
        std::vector<int> validEdges;
        
        for (size_t i = 0; i < edges.size(); ++i) 
        {
            if (edges[i].w <= 0)
            {
                continue;
            }
            
            if (uf.find(edges[i].u) != uf.find(edges[i].v)) 
            {
                totalWeight += edges[i].w;
                prefixWeights.push_back(totalWeight);
                validEdges.push_back(i);

            }
        }
        
        if (totalWeight == 0) 
        {
            break;
        }

        std::uniform_int_distribution<long long> dist(0, totalWeight - 1);

        long long target = dist(rng);
        auto it = std::upper_bound(prefixWeights.begin(), prefixWeights.end(), target);
        
        int idx = std::distance(prefixWeights.begin(), it);
        int edge_idx = validEdges[idx];
        
        uf.unite(edges[edge_idx].u, edges[edge_idx].v);
    }


}

CutResult kargerSteinRecursive(int numOrigNodes, UnionFind uf, const std::vector<Edge>& edges, std::mt19937_64& rng) 
{

    if (uf.numSets <= 2) 
    {
        CutResult result;
        result.side.assign(numOrigNodes, 0);
        
        if (uf.numSets == 2) 
        {

            int root0 = -1;
            for (int i = 0; i < numOrigNodes; ++i) 
            {
                if (uf.parent[i] == i) 
                {

                    if (root0 == -1) 
                    {
                        root0 = i;
                    }
                }
            }
            
            for (int i = 0; i < numOrigNodes; ++i) 
            {
                result.side[i] = (uf.find(i) == root0) ? 0 : 1;
            }
            
            for (const auto& e : edges) 
            {
                if (result.side[e.u] != result.side[e.v]) 
                {
                    result.weight += e.w;

                }
            }
        }
        
        return result;
    }

    int k = uf.numSets;
    int t = static_cast<int>(std::ceil(1.0 + k / std::sqrt(2.0)));

    if (t >= k) 
    {
        t = k - 1;
    }

    if (t < 2) 
    {
        t = 2;
    }

    // branch 1
    UnionFind uf1 = uf;
    kargerSteinContract(uf1, edges, t, rng);
    CutResult res1 = kargerSteinRecursive(numOrigNodes, uf1, edges, rng);

    // branch 2
    UnionFind uf2 = std::move(uf); 
    kargerSteinContract(uf2, edges, t, rng);
    CutResult res2 = kargerSteinRecursive(numOrigNodes, uf2, edges, rng);

    return (res1.weight <= res2.weight) ? res1 : res2;
}

long long kargerSteinMinCut(const Graph& g, std::vector<int>& side, std::uint64_t seed) 
{
    if (g.n <= 0) 
    {
        side.clear();
        return 0;
    }

    if (g.n == 1) 
    {
        side.assign(1, 0);
        return 0;
        
    }
    
    std::mt19937_64 rng(seed);
    UnionFind init_uf(g.n);
    CutResult res = kargerSteinRecursive(g.n, init_uf, g.edges, rng);

    side = res.side;
    return res.weight;
}