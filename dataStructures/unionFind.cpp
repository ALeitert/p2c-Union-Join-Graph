#include "unionFind.h"


// Default constructor.
// Creates an empty data structure.
UnionFind::UnionFind() { /* Does nothing. */ }

// Default constructor.
// Creates an empty data structure.
UnionFind::UnionFind(size_t size) : size(size)
{
    parent = new size_t[size];
    rank = new size_t[size];

    for (size_t i = 0; i < size; i++)
    {
        parent[i] = i;
        rank[i] = 0;
    }
}

// Destructor.
UnionFind::~UnionFind()
{
    if (rank != nullptr) delete[] rank;
    if (parent != nullptr) delete[] parent;
}

void UnionFind::unionSets(size_t x, size_t y)
{
    size_t xRoot = findSets(x);
    size_t yRoot = findSets(y);

    if (xRoot == yRoot) return;


    // x and y are in different sets. Merge them.

    if (rank[xRoot] < rank[yRoot])
    {
        parent[xRoot] = yRoot;
    }
    else if (rank[xRoot] > rank[yRoot])
    {
        parent[yRoot] = xRoot;
    }
    else
    {
        parent[yRoot] = xRoot;
        rank[xRoot]++;
    }
}

size_t UnionFind::findSets(size_t x)
{
    if (parent[x] != x)
    {
        parent[x] = findSets(parent[x]);
    }

    return parent[x];
}
