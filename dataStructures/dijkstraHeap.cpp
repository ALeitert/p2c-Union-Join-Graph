#include <limits>

#include "dijkstraHeap.h"


// Default constructor.
// Creates an empty heap.
DijkstraHeap::DijkstraHeap() { /* Does nothing. */ }

// Constructor.
// Ceates a heap with a given size and directly fills it with "infinite" weight for each vertex.
DijkstraHeap::DijkstraHeap(size_t capacity)
{
    size = capacity;

    // Create arrays.
    verIds = new size_t[capacity];
    weights = new int[capacity];
    indices = new size_t[capacity];

    for (size_t vId = 0; vId < capacity; vId++)
    {
        verIds[vId] = vId;
        weights[vId] = std::numeric_limits<int>::max();
        indices[vId] = vId;
    }
}

// Destructor.
DijkstraHeap::~DijkstraHeap()
{
    if (verIds != nullptr) delete[] verIds;
    if (weights != nullptr) delete[] weights;
    if (indices != nullptr) delete[] indices;
}
