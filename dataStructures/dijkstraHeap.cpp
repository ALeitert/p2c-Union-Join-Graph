#include <limits>
#include <stdexcept>
#include <utility>

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


// Returns the array with the weights of all vertices.
int* const DijkstraHeap::getWeights() const
{
    return weights;
}

// Return the current size of the heap.
size_t DijkstraHeap::getSize() const
{
    return size;
}


void DijkstraHeap::update(size_t vId, int vWei)
{
    if (indices[vId] == std::numeric_limits<size_t>::max())
    {
        std::logic_error("Vertex is not in heap.");
    }

    size_t index = indices[vId];
    weights[vId] = vWei;

    if (index == 0)
    {
        heapify(index);
        return;
    }

    size_t parId = verIds[parent(index)];
    int parWei = weights[parId];

    if (parWei <= vWei)
    {
        heapify(index);
    }
    else
    {
        moveUp(index);
    }
}


// Moves an element down until it has a fitting place.
void DijkstraHeap::heapify(size_t index)
{
    while (true)
    {
        size_t l = left(index);
        size_t r = right(index);

        if (l >= getSize()) return;

        size_t smlIdx = l;
        size_t smlId = verIds[smlIdx];
        int smlWei = weights[smlId];

        if (r < getSize())
        {
            size_t rId = verIds[r];
            int rWei = weights[rId];

            if (rWei < smlWei)
            {
                smlIdx = r;
                smlWei = rWei;
            }
        }

        int idxWei = weights[verIds[index]];

        if (smlWei >= idxWei)
        {
            return;
        }

        swapKeys(smlIdx, index);
        heapify(smlIdx);
    }
}

// Moves an element up until it has a fitting place.
void DijkstraHeap::moveUp(size_t index)
{
    while (index > 0)
    {
        int parIdx = parent(index);

        int parWei = weights[verIds[parIdx]];
        int idxWei = weights[verIds[index]];

        if (parWei <= idxWei)
        {
            return;
        }

        swapKeys(parIdx, index);
        index = parIdx;
    }
}


// Helper function to swap two entries in the heap.
void DijkstraHeap::swapKeys(size_t idx1, size_t idx2)
{
    size_t vId1 = verIds[idx1];
    size_t vId2 = verIds[idx2];

    verIds[idx1] = vId2;
    verIds[idx2] = vId1;

    indices[vId1] = idx2;
    indices[vId2] = idx1;
}
