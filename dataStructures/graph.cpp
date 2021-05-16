#include <stdexcept>

#include "graph.h"


// Default constructor.
Graph::Graph() { /* Does nothing. */ }

// Constructor.
// Constructs a graph from the given edge list.
// Needs to be sorted by vertex-IDs, not contain duplicates, and from-ID > to-ID.
Graph::Graph(const vector<intPair>& eList, const vector<int>& wList)
{
    // --- Verify input. ---

    // Equal length?
    if (eList.size() != wList.size())
    {
        throw logic_error("Size of given listst not equal.");
    }

    // Sorted, no duplicates?
    for (size_t i = 1; i < eList.size(); i++)
    {
        int preF = eList[i - 1].first;
        int preT = eList[i - 1].second;

        int curF = eList[i].first;
        int curT = eList[i].second;

        // We want that the previous entry is strictly smaller than the current.
        // That is the case if and only if
        //     preF < curF || (preF == curF && preT < curT).
        // Subsequently, it is not the case if and only if
        //     preF >= curF && (preF != curF || preT >= curT)
        if (preF >= curF && (preF != curF || preT >= curT))
        {
            throw logic_error("List not sorted or contain duplicates.");
        }
    }

    // from-ID > to-ID?
    for (size_t i = 0; i < eList.size(); i++)
    {
        if (eList[i].first <= eList[i].second)
        {
            throw logic_error("From-ID <= to-ID.");
        }
    }


    // --- Build graph. ---

    // Due to order and from-ID > to-ID, largest ID is from-ID at the very end.
    vSize = (eList.size() == 0 ? 0 : eList.back().first + 1);

    edges = new vector<int>[vSize];
    weights = new vector<int>[vSize];


    for (size_t i = 0; i < eList.size(); i++)
    {
        int fId = eList[i].first;
        int tId = eList[i].second;
        int wei = wList[i];

        edges[fId].push_back(tId);
        edges[tId].push_back(fId);

        weights[fId].push_back(wei);
        weights[tId].push_back(wei);
    }
}


// Destructor.
Graph::~Graph()
{
    if (edges != nullptr) delete[] edges;
    if (weights != nullptr) delete[] weights;
}


// Returns the neighbours of the given vertex.
const vector<int>& Graph::operator[](const int vId) const
{
    if (vId >= vSize) throw out_of_range("vId");

    return edges[vId];
}

// Returns the weights to neighbours of the given vertex.
const vector<int>& Graph::operator()(const int vId) const
{
    if (vId >= vSize) throw out_of_range("vId");

    return weights[vId];
}


// The number of vertices.
size_t Graph::size() const
{
    return vSize;
}
