// Implements a Union-Find data structure.

#ifndef __UnionFind_H__
#define __UnionFind_H__


#include <cstddef>


class UnionFind
{
public:

    // Default constructor.
    // Creates an empty data structure.
    UnionFind();

    // Default constructor.
    // Creates an empty data structure.
    UnionFind(size_t size);

    // Destructor.
    ~UnionFind();


    // Implements Union-operation.
    void unionSets(size_t x, size_t y);

    // Implements Find-operation.
    size_t findSets(size_t x);


private:

    // Number of elements in data structure.
    size_t size = 0;

    // Stores the rank of each element.
    size_t* rank = nullptr;

    // Stores the parent of each element.
    size_t* parent = nullptr;
};

#endif
