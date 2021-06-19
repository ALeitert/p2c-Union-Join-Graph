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


        // --- Algorithm 5 from [2] ---

        //  0  Compute a factorising permutation sigma = < v_1, v_2, ..., v_n >.

        //  1  Add v_0 and v_{n + 1} to sigma (these are dummy vertices which
        //     are never twins with any other vertex).
        //  2  Set z := v_1.
        //  3  Let pre(z) and suc(z) denote the preceeding and succeding
        //     vertices of z in sigma, respectively.

        //  4  While z != v_{n + 1}
        //  5      If z and pre(z) are twins (true or false) in G(sigma) Then
        //  6          Remove pre(z) from sigma.
        //  7      Else If z and suc(z) are twins in G(sigma) Then
        //  8          Set z := suc(z).
        //  9          Remove pre(z) from sigma.
        // 10      Else
        // 11          Set z := suc(z).

        // 12  If |sigma - { v_0, v_{n + 1} }| = 1 Then
        // 13      G is a cograph.
        // 14  Else
        // 15      G contains a P_4.
    }
}
