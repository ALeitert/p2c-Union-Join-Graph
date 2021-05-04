#include "sorting.h"


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
