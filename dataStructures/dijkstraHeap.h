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

    // Constructor.
    // Ceates a heap with a given size and directly fills it with "infinite" weight for each vertex.
    DijkstraHeap(size_t capacity);

    // Destructor.
    ~DijkstraHeap();


    // Returns the array with the weights of all vertices.
    const int* getWeights() const;

    // Return the current size of the heap.
    size_t getSize() const;


    // Returns the ID of the vertex with smalles weight.
    size_t getMinId();

    // Returns the smalles weight.
    int getMinWeight();

    // Removes the vertex with the smalles weight and returns its ID.
    size_t removeMin();


    // Updates the weight of a vertex in the heap.
    void update(size_t vId, int vWei);


private:

    // Moves an element down until it has a fitting place.
    void heapify(size_t index);

    // Moves an element up until it has a fitting place.
    void moveUp(size_t index);


    // Helper functions to compute indices (assuming 0-based indices).
    inline size_t left(size_t index) const { return (index << 1) + 1; }
    inline size_t right(size_t index) const { return (index << 1) + 2; }
    inline size_t parent(size_t index) const { return (index - 1) >> 1; }

    // Helper function to swap two entries in the heap.
    void swapKeys(size_t idx1, size_t idx2);


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
