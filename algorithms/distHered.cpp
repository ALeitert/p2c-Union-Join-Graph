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
    // Types of twins.
    enum class TwinType
    {
        None,
        FalseTwin,
        TrueTwin
    };

    // Determines if two vertices are twins by comparing their neighbourhoods.
    TwinType checkTwins(const Graph& g, int uId, int vId, const vector<bool> ignore)
    {
        const vector<int>& uNei = g[uId];
        const vector<int>& vNei = g[vId];

        bool trueTwins = false;

        for (size_t i = 0, j = 0;; i++, j++)
        {
            // --- Skip ignored vertices and check adjacency. ---

            int niId = -1;
            int njId = -1;

            for (; i < uNei.size(); i++)
            {
                niId = uNei[i];

                if (ignore[niId]) continue;

                if (niId == vId)
                {
                    trueTwins = true;
                }
                else
                {
                    break;
                }
            }

            for (; j < vNei.size(); j++)
            {
                // Simpler loop since we do not need to check for adjacency.
                njId = vNei[j];

                if (ignore[njId]) continue;
                if (njId == uId) continue;

                break;
            }


            // --- Compare neighbours. ---

            // Now, i and j either refer to a neighbour still in G or to
            // the end of their respective neighbourhoods.


            bool iInRange = i < uNei.size();
            bool jInRange = j < vNei.size();


            // If one neighbourhood still contains vertices while the other is
            // completed: not twins.
            if (iInRange != jInRange) return TwinType::None;

            // Both neighbourhoods completed?
            if (!iInRange)
            {
                return trueTwins ? TwinType::TrueTwin : TwinType::FalseTwin;
            }

            // Both neighbours the same?
            if (niId != njId) return TwinType::None;
        }
    }


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
