// Implements a heap for Dijkstra's algorithm.
// Allows to quickly find vertices and update their weight.

#ifndef __DijkstraHeap_H__
#define __DijkstraHeap_H__


#include <cstddef>


class DijkstraHeap
{
public:

    // Default constructor.
    // Creates an empty heap.
    DijkstraHeap();

    // Destructor.
    ~DijkstraHeap();


private:

    // Number of elements stored in heap.
    size_t size = 0;

    // Array storing the heap.
    size_t* verIds = nullptr;

    // Weights of each vertex.
    int* weights = nullptr;

    // Index in heap of each vertex.
    size_t* indices = nullptr;
};

#endif
