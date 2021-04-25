#include <cassert>

#include "hypergraph.h"

// Default constructor.
// Creates an empty hypergraph.
Hypergraph::Hypergraph() : Hypergraph(0, 0)
{
    // Nothing to do.
}

// Constructor.
// Creates a hypergraph based on an edge list.
// Edges go from a hyperede to a vertex.
Hypergraph::Hypergraph(vector<evPair>& eList)
{
    vector<vector<int>> eSet;
    vector<vector<int>> vSet;

    for (int i = 0; i < eList.size(); i++)
    {
        int eId = eList[i].first;
        int vId = eList[i].second;

        while (eSet.size() <= eId) eSet.push_back(vector<int>());
        while (vSet.size() <= vId) vSet.push_back(vector<int>());

        eSet[eId].push_back(vId);
        vSet[vId].push_back(eId);
    }

    initialize(vSet.size(), eSet.size(), eList.size());

    for (int j = 0; j < eSize; j++)
    {
        hyperedges[j] = std::move(eSet[j]);
    }

    for (int i = 0; i < vSize; i++)
    {
        vertices[i] = std::move(vSet[i]);
    }
}

// Constructor.
// Initialises a new hypergraph with n vertices and m hyperedges.
// Does not create any edges in the bipartite representation.
Hypergraph::Hypergraph(const int n, const int m)
{
    initialize(n, m, 0);
}

// Move constructor.
Hypergraph::Hypergraph(Hypergraph&& other)
{
    vSize = other.vSize;
    eSize = other.eSize;
    tSize = other.tSize;

    vertices = other.vertices;
    hyperedges = other.hyperedges;

    other.initialize(0, 0, 0);
}

// Destructor.
Hypergraph::~Hypergraph()
{
    destruct();
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

// Helper function for destructor and similar operations.
// Frees occupied memory.
void Hypergraph::destruct()
{
    delete[] vertices;
    delete[] hyperedges;
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


// Move assignment.
Hypergraph& Hypergraph::operator= (Hypergraph&& other)
{
    destruct();

    vSize = other.vSize;
    eSize = other.eSize;
    tSize = other.tSize;

    vertices = other.vertices;
    hyperedges = other.hyperedges;

    other.initialize(0, 0, 0);

    return *this;
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
