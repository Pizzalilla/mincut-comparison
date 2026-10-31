#ifndef MINCUT_UNION_FIND_HPP
#define MINCUT_UNION_FIND_HPP

#include <vector>

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

#endif // MINCUT_UNION_FIND_HPP