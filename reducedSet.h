// Implements a data structure to represent sets in a reduced way as defined in Pritchard, Algorithmica 1999.

#ifndef __ReducedSet_H__
#define __ReducedSet_H__

#include <cstdint>
#include <utility>
#include <vector>

using namespace std;


class ReducedSet
{
    // Consider a bit-array A which represents a subset of some set F.
    // A ReducedSet-object represents A in a more memory efficient version.

    // We assume 32-bit words.
    typedef uint32_t word;

    // Allows to manage non-zero words and their indices.
    typedef pair<int, word> wordIndex;

public:

    // Default constructor.
    // Creates an invalid set.
    ReducedSet() { }

    // Copy constructor.
    ReducedSet(const ReducedSet& set);

    // Constructor.
    // Creates a set from a list of elements.
    // Assumes that the given list is sorted.
    ReducedSet(const vector<int>& list);

    // Destructor.
    ~ReducedSet();


    // Computes the intersection of two sets and returns the result as a new set.
    ReducedSet operator&(const ReducedSet& rhs) const;

    // Changes the current set to be the intersection of the current and given set.
    void operator&=(const ReducedSet& rhs);


private:

    // Constructor.
    // Creates a set of the given size that uses the given array.
    ReducedSet(int size, wordIndex* arr) : n(size), R(arr) { }


    // The number of nonzero words in the original array A.
    int n = -1;

    // The non-zero words in array A and their respective indices in A.
    wordIndex* R = nullptr;
};

#endif
