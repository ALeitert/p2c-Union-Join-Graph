// Implements a data structure to represent sets in a reduced way as defined in Pritchard, Algorithmica 1999.

#ifndef __ReducedSet_H__
#define __ReducedSet_H__

#include <cstdint>
#include <iterator>
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

    // Move constructor.
    ReducedSet(ReducedSet&& set);

    // Constructor.
    // Creates a set from a list of elements.
    // Assumes that the given list is sorted.
    ReducedSet(const vector<int>& list);

    // Destructor.
    ~ReducedSet();


    // Move assignment.
    ReducedSet& operator=(ReducedSet&& set);


    // Computes the intersection of two sets and returns the result as a new set.
    ReducedSet operator&(const ReducedSet& rhs) const;

    // Changes the current set to be the intersection of the current and given set.
    void operator&=(const ReducedSet& rhs);


    // Allows to iterate over all elements stored in the set.
    class Iterator;


private:

    // Constructor.
    // Creates a set of the given size that uses the given array.
    ReducedSet(int size, wordIndex* arr) : n(size), R(arr) { }


    // The number of nonzero words in the original array A.
    int n = -1;

    // The non-zero words in array A and their respective indices in A.
    wordIndex* R = nullptr;
};


// Allows to iterate over all elements stored in the set.
class ReducedSet::Iterator : iterator<input_iterator_tag, int>
{
    // ToDo: Implement required constructors and operators.
    //   - copy constructor and assignment
    //   - destructor
    //   - increment operator, prefix (++it) and postfix (it++)
    //   - equality (==) and inequality (!=) comparisons
    //   - dereference as righ-hand side value: *it and it-> (-> returns int*)

    // See for more infos:
    //   https://stackoverflow.com/a/8054856
    //   http://www.cplusplus.com/reference/iterator/
};

#endif
