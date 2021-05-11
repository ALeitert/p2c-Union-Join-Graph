// Implements a data structure to mange sets for maximum cardinality search.

#ifndef __MaxCardinalitySet_H__
#define __MaxCardinalitySet_H__


#include <vector>

#include "../helper.h"


using namespace std;


class MaxCardinalitySet
{
public:

    // Default constructor.
    // Creates empty set.
    MaxCardinalitySet();

    // Contructor.
    // Creates the data structure with the given number of sets.
    // Set IDs are in [0, k). Each set has size 0.
    MaxCardinalitySet(size_t k);

private:

    // Contains the stored sets in order of their size.
    vector<int> setList;

    // States the index and size of each set.
    vector<sizePair> setInfo;

    // The start indices of each group of equal size.
    vector<size_t> startIndices;

};

#endif
