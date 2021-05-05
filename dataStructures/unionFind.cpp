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
