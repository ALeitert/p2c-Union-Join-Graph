#include <stdexcept>

#include "graph.h"


// Default constructor.
Graph::Graph() { /* Does nothing. */ }

// Destructor.
Graph::~Graph()
{
    if (edges != nullptr) delete[] edges;
    if (weights != nullptr) delete[] weights;
}


// Returns the neighbours of the given vertex.
const vector<size_t>& Graph::operator[](const size_t vId) const
{
    if (vId >= vSize) throw out_of_range("vId");

    return edges[vId];
}

// Returns the weights to neighbours of the given vertex.
const vector<int>& Graph::operator()(const size_t vId) const
{
    if (vId >= vSize) throw out_of_range("vId");

    return weights[vId];
}


// The number of vertices.
size_t Graph::size() const
{
    return vSize;
}
