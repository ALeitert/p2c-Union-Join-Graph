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

    endIndices.push_back(k - 1);
}


// Increases the size of the set with the given ID by 1.
void MaxCardinalitySet::increaseSize(int setId)
{
    // Given set.
    size_t& idx = setInfo[setId].first;
    size_t& size = setInfo[setId].second;

    // Last set with same size.
    size_t last = endIndices[size];
    int lastId = setList[last];
    size_t& lastIdx = setInfo[lastId].first;

    // Swap elements.
    swap(setList[idx], setList[lastIdx]);
    swap(idx, lastIdx);

    // Increase size.
    endIndices[size]--;
    size++;

    // Add range for new size if it not exist yet.
    if (size >= endIndices.size())
    {
        endIndices.push_back(idx);
    }
}
