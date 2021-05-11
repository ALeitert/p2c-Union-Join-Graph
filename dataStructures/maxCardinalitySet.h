// Implements a data structure to mange sets for maximum cardinality search.

#ifndef __MaxCardinalitySet_H__
#define __MaxCardinalitySet_H__


#include <vector>

#include "../helper.h"


using namespace std;


class MaxCardinalitySet
{
public:


private:

    // Contains the stored sets in order of their size.
    vector<int> setList;

    // States the index and size of each set.
    vector<sizePair> setInfo;

    // The start indices of each group of equal size.
    vector<size_t> startIndices;

};


#endif
