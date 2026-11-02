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
#include "union_find.hpp"

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

Graph buildCompactGraph(const Graph& original_g, UnionFind& uf, std::vector<int>& root_to_compact)
{
    Graph compactG;
    compactG.n = uf.numSets;
    
    int current_idx = 0;
    for (int i = 0; i < original_g.n; ++i) 
    {
        if (uf.parent[i] == i) 
        {
            root_to_compact[i] = current_idx++;
        }
    }

    struct CompactEdge { int u, v; long long w; };
    std::vector<CompactEdge> active_edges;
    active_edges.reserve(original_g.edges.size());

    for (const auto& e : original_g.edges) 
    {
        if (e.w <= 0) continue;
        int ru = uf.find(e.u);
        int rv = uf.find(e.v);
        
        if (ru != rv) 
        {
            int cu = root_to_compact[ru];
            int cv = root_to_compact[rv];
            if (cu > cv) std::swap(cu, cv);
            active_edges.push_back({cu, cv, e.w});
        }
    }

    std::sort(active_edges.begin(), active_edges.end(), [](const CompactEdge& a, const CompactEdge& b) {
        if (a.u != b.u) return a.u < b.u;
        return a.v < b.v;
    });

    for (const auto& e : active_edges) 
    {
        if (!compactG.edges.empty() && compactG.edges.back().u == e.u && compactG.edges.back().v == e.v) 
        {
            compactG.edges.back().w += e.w;
        } 
        else 
        {
            compactG.edges.push_back({e.u, e.v, e.w});
        }
    }
    
    return compactG;
}

CutResult kargerSteinRecursive(const Graph& g, std::mt19937_64& rng) 
{

    if (g.n <= 2) 
    {
        CutResult result;
        result.side.assign(g.n, 0);
        
        if (g.n == 2) 
        {

            result.side[1] = 1;
            for (const auto& e : g.edges) 
            {
                if (result.side[e.u] != result.side[e.v]) 
                {
                    result.weight += e.w;
                }
            }
        }

        return result;  
        
    }

    long long totalW = 0;
    for (const auto& e : g.edges) 
    {
        totalW += e.w;
    }


    if (totalW == 0) 
    {
        CutResult result;

        result.side.assign(g.n, 0);
        result.side[0] = 1; 
        result.weight = 0;
        return result;
    }

    int t = static_cast<int>(std::ceil(1.0 + g.n / std::sqrt(2.0)));
    if (t >= g.n)
    {
       t = g.n - 1; 
    }

    if (t < 2)
    {
       t = 2;   
    }

    CutResult res1;
    {
        UnionFind uf1(g.n);
        kargerSteinContract(uf1, g.edges, t, rng);
        
        std::vector<int> root_to_compact(g.n, -1);
        Graph g1 = buildCompactGraph(g, uf1, root_to_compact);
        
        CutResult sub_res = kargerSteinRecursive(g1, rng);
        
        // Map sides back to original vertices
        res1.weight = sub_res.weight;
        res1.side.assign(g.n, 0);
        for (int i = 0; i < g.n; ++i) 
        {
            res1.side[i] = sub_res.side[root_to_compact[uf1.find(i)]];
        }
    }

    // --- Branch 2 ---
    CutResult res2;
    {
        UnionFind uf2(g.n);
        kargerSteinContract(uf2, g.edges, t, rng);
        
        std::vector<int> root_to_compact(g.n, -1);
        Graph g2 = buildCompactGraph(g, uf2, root_to_compact);
        
        CutResult sub_res = kargerSteinRecursive(g2, rng);
        
        res2.weight = sub_res.weight;
        res2.side.assign(g.n, 0);
        for (int i = 0; i < g.n; ++i) 
        {
            res2.side[i] = sub_res.side[root_to_compact[uf2.find(i)]];
        }
    }

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
    CutResult res = kargerSteinRecursive(g, rng);

    side = res.side;
    return res.weight;
}

long long kargerSteinSingleRun(const Graph& g, std::vector<int>& side, std::uint64_t seed) 
{
    return kargerSteinMinCut(g, side, seed);
}