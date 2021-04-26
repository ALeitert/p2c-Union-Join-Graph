// Contains various supporting functions.

#ifndef __Helper_H__
#define __Helper_H__

#include <utility>
#include <vector>

using namespace std;


// Sorts a set of integer pairs using radix sort.
// Requires O(n + k) additional memory.
void sortPairsRadix(vector<pair<int, int>>& pairs);

#endif
