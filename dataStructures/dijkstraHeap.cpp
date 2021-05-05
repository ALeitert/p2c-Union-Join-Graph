#include "dijkstraHeap.h"


// Default constructor.
// Creates an empty heap.
DijkstraHeap::DijkstraHeap() { /* Does nothing. */ }

// Destructor.
DijkstraHeap::~DijkstraHeap()
{
    if (verIds != nullptr) delete[] verIds;
    if (weights != nullptr) delete[] weights;
    if (indices != nullptr) delete[] indices;
}
