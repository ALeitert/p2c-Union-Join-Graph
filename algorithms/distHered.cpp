#include "distHered.h"


// --- Papers ---

// [1] Damiand, Habib, Paul:
//     A simple paradigm for graph recognition: application to cographs and distance hereditary graphs.
//     Theoretical Computer Science 263, 99 - 111, 2001.

// [2] Habib, Paul:
//     A simple linear time algorithm for cograph recognition.
//     Discrete Applied Mathematics 145, 183 - 197, 2005.


// Anonymous namespace for helper functions.
namespace
{
    // Determines a factorising permutation if the given graph is a cograph.
    vector<int> factPermutation(const Graph& g)
    {
        throw runtime_error("Not implemented.");

        // Algorithm 2 from [2].
    }

    // Computes a cotree for the given graph if it is a cograph.
    // Otherwise nothing is returned.
    void cotree(const Graph& g)
    {
        // ToDo: return type.
        throw runtime_error("Not implemented.");

        // Algorithm 5 from [2].
    }
}
