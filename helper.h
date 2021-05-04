// Contains various supporting functions.

#ifndef __Helper_H__
#define __Helper_H__

#include <limits>
#include <ostream>
#include <utility>
#include <vector>

using namespace std;


typedef pair<int, int> intPair;
static constexpr intPair MaxIntPair = intPair(numeric_limits<int>::max(), numeric_limits<int>::max());

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


// Prints a list of integer pairs to the terminal.
void print(const vector<intPair>& pairs);

// Prints the given time (in milliseconds) into the given stream using appropriate unit.
void printTime(int64_t time, ostream& out);


void makePermutation(int* arr, int size);

// Checks if the given vector is sorted; if not it creates a sorted copy.
vector<intPair>* ensureSorting(const vector<intPair>& vec);

// Checks if the given vector is sorted and sorts it if not.
void ensureSorting(vector<intPair>& vec);

#endif
