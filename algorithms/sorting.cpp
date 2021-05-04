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

// Lexicographically sorts the given list of vectors.
// Returns an array A[] such that A[i] is the ID of the vector which is at position i in a lex. order.
size_t* Sorting::lexSort(const vector<vector<int>>& lst)
{
    // We implement an algorithm presented in [1]. It allows to lexicographically sort
    // strings with n total characters from an alphabet of size m in O(n + m) time.

    // [1] Aho, Hopcroft, Ullman:
    //     The Design and Analysis of Computer Algorithms.
    //     Addison-Wesley, 1974.



    // Phase 1: Determine which characters appear at which position.

    // Step 1.1: Collect all characters and sort them by their value and their position.
    // That is, for each position p (with 1 <= p <= L) and each characters c at position p,
    // create a pair (p, c) and sort all these pairs using radix sort. Note that the
    // base is changing.


    // -- Determine total and maximum length. --

    size_t lstSize = lst.size();
    size_t totalLength = 0;
    size_t maxLength = 0;

    for (int i = 0; i < lstSize; i++)
    {
        totalLength += lst[i].size();
        maxLength = max(maxLength, lst[i].size());
    }


    // -- Build pairs and sort them. --

    vector<intPair> pcPairs;
    for (int i = 0; i < lstSize; i++)
    {
        const vector<int>& str = lst[i];

        for (int p = 0; p < str.size(); p++)
        {
            pcPairs.push_back(intPair(p, str[p]));
        }
    }

    Sorting::radixSort(pcPairs);


    // Step 1.2: Remove dublicates and partition by length.

    // In [1], NonEmpty only contais the set of characters that are used for that position.
    // We additionally count how often each appears and then calculte the prefix-sums.

    vector<vector<intPair>> NonEmpty;

    for (int p = 0, i = 0 /* index in list */; p < maxLength; p++) // Note that each p exists.
    {
        NonEmpty.push_back(vector<intPair>());
        vector<intPair>& neVec = NonEmpty[p];


        // Iterate over all characters with same position.
        for (int lastC = -1; i < totalLength && pcPairs[i].first == p; i++)
        {
            int c = pcPairs[i].second;

            if (c != lastC)
            {
                neVec.push_back(intPair(c, 0));
                lastC = c;
            }

            neVec.back().second++;
        }

        // Compute prefix sums.
        for (int j = 1; j < neVec.size(); j++)
        {
            neVec[j].second += neVec[j - 1].second;
        }
    }


    // Phase 2: Raddix sort.

    // Since we sort strings, we start sorting at the last position.
    // Shorter strings are added later and always at the beginning.

    // Note that only sort indices and do not rearange the given array.


    // Step 2.1: Sort by length.

    // Arrays to store orders.
    // No need to initialise, since we never read before writing them completely.
    size_t* orgOrder = new size_t[lstSize];
    size_t* newOrder = new size_t[lstSize];


    // We use counting sort.
    int lenCount[maxLength + 1]; // +1 since indices start at and lengths at 1.
    for (int i = 0; i <= maxLength; i++) lenCount[i] = 0;

    // Count.
    for (size_t sIdx = 0; sIdx < lstSize; sIdx++)
    {
        int key = lst[sIdx].size();
        lenCount[key]++;
    }

    // Prefix sums.
    for (int i = 1; i <= maxLength; i++)
    {
        lenCount[i] += lenCount[i - 1];
    }

    // We need that for later.
    // Makes it easy to determine start and end of each group of lengths.
    int lenRange[maxLength + 1];
    copy(lenCount, lenCount + maxLength + 1, lenRange);

    // Sort.
    for (size_t sIdx = lstSize - 1; sIdx < numeric_limits<size_t>::max(); sIdx--)
    {
        int key = lst[sIdx].size();
        lenCount[key]--;
        int oIdx = lenCount[key];
        orgOrder[oIdx] = sIdx;
    }

    // orgOrder[] now represents the given strings sorted by length.


    // Step 2.2: Sort strings.

    // Allows to simply swap both after each iteration, istead of copying numbers back.
    copy(orgOrder, orgOrder + lstSize, newOrder);


    // It is important that cCount is not cleared inside the loop below.
    // Doing so would result in O(b) extra runtime for each digit and, therefore, in
    // O(n * b) total time. The whole purpose of this approach is to avoid that.
    vector<int> cCount;

    for (int pos = maxLength - 1; pos >= 0; pos--)
    {
        int beg = lenRange[pos];
        int end = lstSize; // exclusive


        // Shortened counting sort:
        // We do not need to count nore compute prefix sums.
        // These values are already stored in NonEmpty;

        // Update counting array.
        vector<intPair>& lenVec = NonEmpty[pos];
        for (const intPair& pair : lenVec)
        {
            const int& c = pair.first;

            if (cCount.size() <= c) cCount.resize(c + 1);
            cCount[c] = pair.second;
        }

        // Sort.
        for (int i = end - 1; i >= beg; i--)
        {
            size_t sIdx = orgOrder[i];
            int chr = lst[sIdx][pos];

            cCount[chr]--;
            int oIdx = cCount[chr] + beg;

            newOrder[oIdx] = sIdx;
        }

        // Swap pointers to do other directin in next iteration.
        swap(orgOrder, newOrder);
    }

    // Lexicographical order is now in orgOrder[] (due to swapping).

    delete[] newOrder;
    return orgOrder;
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
