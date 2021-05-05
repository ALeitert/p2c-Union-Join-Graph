// Implements a data structure to represent sets in a reduced way as defined in Pritchard, Algorithmica 1999.

#ifndef __ReducedSet_H__
#define __ReducedSet_H__

#include <iterator>
#include <utility>
#include <vector>

using namespace std;


class ReducedSet
{
    // Consider a bit-array A which represents a subset of some set F.
    // A ReducedSet-object represents A in a more memory efficient version.

    // Word size of local machine.
    // See https://stackoverflow.com/q/35843365.
    typedef size_t word;

    // The values below allow to transform between bit representation and represented numbers with bit operations instead of multiplication, division, and modulo.

    // See https://stackoverflow.com/a/11376759.
    #define LOG2(X) ((unsigned) ((sizeof(unsigned long long) << 3) - __builtin_clzll((X)) - 1))

    // Size of a word of the local machine reduced to the next power of 2.
    // The +3 effectivly multiplies with 8 to get bits instead of bytes.
    static const unsigned int WordSize = 1 << (LOG2(sizeof(word)) + 3);
    static const unsigned int WordDiv = LOG2(WordSize);
    static const unsigned int WordMod = WordSize - 1;

    #undef LOG2


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


    // Copy assignment.
    ReducedSet& operator=(const ReducedSet& set);

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
    // See for more infos:
    //   https://stackoverflow.com/a/8054856
    //   http://www.cplusplus.com/reference/iterator/

public:

    // Default constructor.
    // Creates an invalid iterator.
    Iterator() { }

    // Destructor.
    // Nothing to do.
    ~Iterator() { }


    // Copy constructor.
    Iterator(const Iterator& it);

    // Copy assignment.
    Iterator& operator=(const Iterator& rhs);


    // Prefix increment operator.
    Iterator& operator++();

    // Postfix increment operator.
    Iterator operator++(int);


    // Dereference operator.
    int operator*() const;


    // Equality comparison.
    bool operator==(const Iterator& rhs) const;

    // Inequality comparison.
    bool operator!=(const Iterator& rhs) const;


    // Creates an iterator that points to the beginning of a given set.
    // Is equal to end if set is empty or invalid.
    static Iterator begin(const ReducedSet& set);

    // Creates an iterator that points to the end of a given set.
    static const Iterator end(const ReducedSet& set);


private:

    // Helper function that finds the next entry in the set.
    void findNext();


    // Points to the array with data.
    wordIndex* ptr = nullptr;

    // Remaining amount of data.
    // Allows to determine when end is reached.
    int length = -1;

    // The index of the current bit in the word.
    int bitIdx = 0;
};

#endif
