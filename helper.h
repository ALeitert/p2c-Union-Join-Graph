// Contains various supporting functions.

#ifndef __Helper_H__
#define __Helper_H__

#include <utility>
#include <vector>

using namespace std;


typedef pair<int, int> intPair;

// Based on https://www.techiedelight.com/use-pair-key-std-unordered_set-cpp/
// Allows to use evPair with unordered_set class.
class intPairHash
{
    static const size_t shift = sizeof(size_t) / 2;
    static const size_t low = (1 << shift) - 1;
    static const size_t high = low << shift;

public:
    size_t operator() (intPair const &pair) const;
};


// Sorts a set of integer pairs using radix sort.
// Requires O(n + k) additional memory.
void sortPairsRadix(vector<intPair>& pairs);

#endif
