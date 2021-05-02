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

    // Creates an iterator pointing to the first element of the set.
    Iterator begin() const;

    // Creates an iterator pointing to the end of the set.
    const Iterator end() const;


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
    //   - destructor
    //   - increment operator, prefix (++it) and postfix (it++)
    //   - equality (==) and inequality (!=) comparisons
    //   - dereference as righ-hand side value: *it and it-> (-> returns int*)

    // See for more infos:
    //   https://stackoverflow.com/a/8054856
    //   http://www.cplusplus.com/reference/iterator/

public:

    // Default constructor.
    // Creates an invalid iterator.
    Iterator() { }


    // Copy constructor.
    Iterator(const Iterator& it);

    // Copy assignment.
    Iterator& operator=(const Iterator& rhs);


    // Creates an iterator that points to the beginning of a given set.
    // Is equal to end if set is empty or invalid.
    static Iterator begin(const ReducedSet& set);

    // Creates an iterator that points to the end of a given set.
    static const Iterator end(const ReducedSet& set);


private:

    // Points to the array with data.
    wordIndex* ptr = nullptr;

    // Remaining amount of data.
    // Allows to determine when end is reached.
    int length = -1;

    // The index of the current bit in the word.
    int bitIdx = 0;
};

#endif
