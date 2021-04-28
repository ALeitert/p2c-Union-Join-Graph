#include <algorithm>

#include "helper.h"


size_t intPairHash::operator() (intPair const &pair) const
{
    size_t h1 = hash<int>()(pair.first);
    size_t h2 = hash<int>()(pair.second);

    h1 = ((h1 << shift) & high) | ((h1 >> shift) & low);

    return h1 ^ h2;
}


// Sorts a set of integer pairs using radix sort.
// Requires O(n + k) additional memory.
void sortPairsRadix(vector<intPair>& pairs)
{
    // --- Counting sort on second dimension. ---

    // Count keys.
    vector<int> count;
    for (int i = 0; i < pairs.size(); i++)
    {
        int key = pairs[i].second;

        while (count.size() <= key) count.push_back(0);

        count[key]++;
    }

    // Prefix sum.
    for (int i = 1; i < count.size(); i++)
    {
        count[i] += count[i - 1];
    }

    // Sort.
    pair<int, int> buffer[pairs.size()];
    for (int i = pairs.size() - 1; i >= 0; i--)
    {
        int key = pairs[i].second;

        count[key]--;
        int idx = count[key];

        buffer[idx] = pairs[i];
    }


    // --- Counting sort on first dimension. ---

    // Count keys.
    count.clear();
    for (int i = 0; i < pairs.size(); i++)
    {
        int key = buffer[i].first;

        while (count.size() <= key) count.push_back(0);

        count[key]++;
    }

    // Prefix sum.
    for (int i = 1; i < count.size(); i++)
    {
        count[i] += count[i - 1];
    }

    // Sort.
    for (int i = pairs.size() - 1; i >= 0; i--)
    {
        int key = buffer[i].first;

        count[key]--;
        int idx = count[key];

        pairs[idx] = buffer[i];
    }
}

void makePermutation(int* arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = i;
    }

    // No idea why std::begin()/end() do not work.
    random_shuffle(arr, arr + size);
}


bool isSorted(const vector<intPair>& vec)
{
    for (int i = 1; i < vec.size(); i++)
    {
        if (vec[i - 1] > vec[i]) return false;
    }

    return true;
}

// Checks if the given vector is sorted; if not it creates a sorted copy.
vector<intPair>* ensureSorting(const vector<intPair>& vec)
{
    if (isSorted(vec)) return nullptr;

    vector<intPair>* newVec = new vector<intPair>(vec);
    sortPairsRadix(*newVec);

    return newVec;
}

// Checks if the given vector is sorted and sorts it if not.
void ensureSorting(vector<intPair>& vec)
{
    if (isSorted(vec)) sortPairsRadix(vec);
}
