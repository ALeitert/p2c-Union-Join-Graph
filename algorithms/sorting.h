// Contains algorithms and helper functions for sorting (in a broad sense).

#ifndef __Algorithms_Sorting_H__
#define __Algorithms_Sorting_H__


#include "../helper.h"


namespace Sorting
{
    // Determines if the given vector is sorted.
    // Requires that the smaller than operator (<) is mplemented.
    template<typename T>
    bool isSorted(const vector<T>& vec);

    // Checks if the given vector is sorted; if not it creates a sorted copy.
    vector<intPair>* ensureSorting(const vector<intPair>& vec);

    // Checks if the given vector is sorted and sorts it if not.
    void ensureSorting(vector<intPair>& vec);


    // Sorts a set of integer pairs using radix sort.
    void radixSort(vector<intPair>& pairs);


    // Creates a random permutation of integers in range [0, size) in the given array.
    void makePermutation(int* arr, size_t size);
}

#endif
