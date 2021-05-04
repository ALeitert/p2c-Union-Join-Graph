#include <algorithm>

#include "sorting.h"


// Determines if the given vector is sorted.
// Requires that the smaller than operator (<) is mplemented.
template<typename T>
bool Sorting::isSorted(const vector<T>& vec)
{
    for (int i = 1; i < vec.size(); i++)
    {
        if (vec[i] < vec[i - 1]) return false;
    }

    return true;
}

// Checks if the given vector is sorted; if not it creates a sorted copy.
vector<intPair>* Sorting::ensureSorting(const vector<intPair>& vec)
{
    if (isSorted(vec)) return nullptr;

    vector<intPair>* newVec = new vector<intPair>(vec);
    Sorting::radixSort(*newVec);

    return newVec;
}

// Checks if the given vector is sorted and sorts it if not.
void Sorting::ensureSorting(vector<intPair>& vec)
{
    if (isSorted(vec)) Sorting::radixSort(vec);
}


// Sorts a set of integer pairs using radix sort.
void Sorting::radixSort(vector<intPair>& pairs)
{
    // --- Counting sort on second dimension. ---

    // Count keys.
    vector<int> count;
    for (int i = 0; i < pairs.size(); i++)
    {
        int key = pairs[i].second;
        if (count.size() <= key) count.resize(key + 1, 0);
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
        if (count.size() <= key) count.resize(key + 1, 0);
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


// Creates a random permutation of integers in range [0, size) in the given array.
void Sorting::makePermutation(int* arr, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = i;
    }

    random_shuffle(arr, arr + size);
}
