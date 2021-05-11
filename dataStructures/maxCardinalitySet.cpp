#include "maxCardinalitySet.h"


// Default constructor.
// Creates empty set.
MaxCardinalitySet::MaxCardinalitySet() { /* Do nothing. */ }

// Contructor.
// Creates the data structure with the given number of sets.
MaxCardinalitySet::MaxCardinalitySet(size_t k)
{
    // --- Create k empty sets. ---

    setList.resize(k);
    setInfo.resize(k);

    for (int i = 0; i < k; i++)
    {
        setList[i] = i;
        setInfo[i] = sizePair(i, 0);
    }

    endIndices.push_back(k);
}
