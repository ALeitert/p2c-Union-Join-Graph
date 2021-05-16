#include <cassert>

#include "../algorithms/sorting.h"
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
Hypergraph::Hypergraph(const vector<intPair>& eList)
{
    // Ensure input is sorted.
    vector<intPair>* ptr = Sorting::ensureSorting(eList);
    const vector<intPair>& list = (ptr == nullptr ? eList : *ptr);

    vector<vector<int>> eSet;
    vector<vector<int>> vSet;

    eSet.resize(list.back().first + 1);

    for (int i = 0; i < list.size(); i++)
    {
        int eId = list[i].first;
        int vId = list[i].second;

        if (vSet.size() <= vId) vSet.resize(vId + 1);

        eSet[eId].push_back(vId);
        vSet[vId].push_back(eId);
    }

    initialize(vSet.size(), eSet.size(), list.size());

    for (int j = 0; j < eSize; j++)
    {
        hyperedges[j] = std::move(eSet[j]);
    }

    for (int i = 0; i < vSize; i++)
    {
        vertices[i] = std::move(vSet[i]);
    }

    if (ptr != nullptr) delete ptr;
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
Hypergraph& Hypergraph::operator=(Hypergraph&& other)
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


// Prints the hypergraph as family of sets.
void Hypergraph::print(ostream& out) const
{
    for (int eId = 0; eId < eSize; eId++)
    {
        out << eId << ":";

        const vector<int>& vList = hyperedges[eId];
        for (int i = 0; i < vList.size(); i++)
        {
            out << " " << vList[i];
        }

        out << endl;
    }
}


// Computes the weighted linegraph of the hypergraph.
Graph Hypergraph::getLinegraph() const
{
    // We compute the linegraph by running a BFS-ish search on each hyperedge in
    // the incidence graph. The search is limited to two hops. Each hyperedge
    // the search reaches is counted in a table. That count becomes the weight
    // of the corresponding linegraph edge.

    const size_t m = getESize();

    vector<vector<intPair>> edgeList;
    edgeList.resize(m);

    size_t totalSize = 0;

    vector<size_t> map;
    map.resize(m, 0);

    vector<int> inMap;


    for (int toId = 0; toId < m; toId++)
    {
        const vector<int>& vList = operator[](toId);

        for (const int& vId : vList)
        {
            const vector<int>& eList = operator()(vId);

            for (size_t i = eList.size() - 1; i < eList.size(); i--)
            {
                int frId = eList[i];
                if (frId <= toId) break;

                if (map[frId] == 0)
                {
                    inMap.push_back(frId);
                }

                map[frId]++;
            }
        }

        for (int& frId : inMap)
        {
            int weig = map[frId];
            map[frId] = 0;

            edgeList[frId].push_back(intPair(toId, weig));
        }

        totalSize += inMap.size();
        inMap.clear();
    }

    vector<intPair> pairList;
    vector<int> weigList;

    pairList.resize(totalSize);
    weigList.resize(totalSize);

    size_t ptr = 0;

    for (int frId = 0; frId < m; frId++)
    {
        for (size_t i = 0; i < edgeList[frId].size(); i++)
        {
            int toId = edgeList[frId][i].first;
            int weig = edgeList[frId][i].second;

            pairList[ptr] = intPair(frId, toId);
            weigList[ptr] = weig;

            ptr++;
        }
    }

    return Graph(pairList, weigList);
}
