#include <cassert>

#include "hypergraph.h"

// Default constructor.
// Creates an empty hypergraph.
Hypergraph::Hypergraph() : Hypergraph(0, 0)
{
    // Nothing to do.
}

// Constructor.
// Initialises a new hypergraph with n vertices and m hyperedges.
// Does not create any edges in the bipartite representation.
Hypergraph::Hypergraph(const int n, const int m)
{
    initialize(n, m, 0);
}

// Destructor.
Hypergraph::~Hypergraph()
{
    delete[] vertices;
    delete[] hyperedges;
}


// Helper function for constructors.
// Initialises a new hypergraph with n vertices and m hyperedges,
// and sets the total size to N.
// Does not create any edges in the bipartite representation.
void Hypergraph::initialize(const int n, const int m, const int N)
{
    vSize = n;
    eSize = m;
    tSize = N;

    vertices = new vector<int>[vSize];
    hyperedges = new vector<int>[eSize];
}


// The number n of vertices.
int Hypergraph::getVSize() const
{
    return vSize;
}

// The number m of hyperedges.
int Hypergraph::getESize() const
{
    return eSize;
}

// The combined size of all hyperedges.
int Hypergraph::getTotalSize() const
{
    return tSize;
}

// Returns the hyperedge with index i.
const vector<int>& Hypergraph::operator[](const int i) const
{
    assert(i >= 0 && i < eSize);
    return hyperedges[i];
}

// Returns the hyperedges containing the vertex with index j.
const vector<int>& Hypergraph::operator()(const int j) const
{
    assert(j >= 0 && j < vSize);
    return vertices[j];
}
