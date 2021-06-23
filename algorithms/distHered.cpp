#include "distHered.h"


#include "../dataStructures/partRefine.h"


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

    // A modifies partition refinement data structure for the recognition of cographs.
    class CographPR : public PartRefinement
    {

    };


    // Determines a factorising permutation if the given graph is a cograph.
    vector<int> factPermutation(const Graph& g)
    {
        // --- Algorithm 2 from [2] ---

        // Rule 1 Refinement:
        // For a given part C and vertex x in C, split C into
        // [ co-N(x) \cap C, { x }, N(x) \cap C ].

        // Rule 2 Refinement:
        // For a given part C and vertex y not in C, split C into
        // [ co-N(y) \cap C, N(y) \cap C ].

        //  1  Let P = [V].
        //  2  Chose an arbitrary vertex o as Origin.
        //  3  If o is an isolated vertex or a universal vertex Then
        //  4      Recurse on G - o.
        //  5  While there exist some non-singleton parts
        //  6      If C_o is not a singleton Then
        //  7          Use rule 1 on C_o with o as pivot.
        //  8          Set co-N(o) \cap C_o and N(o) \cap C_o as unused parts.
        //  9      While there exist unused parts
        // 10          Pick an arbitrary unused part C and an arbitrary vertex y
        //             in C.
        // 11          Set y as the pivot of C.
        // 12          Refine the parts C' != C of P with rule 2 using the
        //             pivot set N(y).
        // 13          Mark C as used and the new created subparts without pivot
        //             as unused.
        // 14      Let z_l and z_r be the pivots of the nearest non-singleton
        //         parts to o respectively to its left and on its right.
        // 15      If z_l is adjacent to z_r Then
        // 16          Set o := z_l.
        // 17      Else
        // 18          Set o := z_r.
        // 19  Return P.

        // In order to efficiently implement step 4 [lines 14 - 18] of
        // Algorithm 2, each time a vertex of a singleton part C is used as
        // pivot with rule 2, the part C is removed from the lists of parts.
        // Also, when the origin of the partition changes, the part containing
        // the old origin is removed from the lists of parts. Therefore, to
        // choose the new origin, we just have to look at the pivots of the two
        // parts adjacent to the part containing the origin.


        size_t n = g.size();


        // --- Line 1 ---

        PartRefinement P(n);


        // --- Initialise management of used groups. ---

        // All unused groups (in no particular order).
        vector<size_t> unusedGrps = { 0 };

        // States the index of a group in the list of used groups.
        // Unused groups have value -1.
        vector<int> unusedPos(n, -1);
        unusedPos[0] = 0;

        // The pivot of each group.
        vector<int> pivot(n, -1);


        // --- Line 2 ---

        int oId = 0;


        // --- Line 5 ---

        while (P.dropSingles())
        {
            // --- Line 6 ---

            if (!P.isDroppedOrSingle(oId))
            {
                // --- Line 7 ---

                vector<size_t> r1Grps = P.r1Refine(oId, g[oId]);


                // --- Line 8 ---

                // According to Procedure 3 in [2], { x } is marked as used.
                size_t xGrp = r1Grps[0];
                int xGrpPos = unusedPos[xGrp];

                if (xGrpPos >= 0)
                {
                    // Swap with last group.
                    swap(unusedGrps[xGrpPos], unusedGrps.back());
                    size_t lastGrp = unusedGrps[xGrpPos];
                    unusedPos[lastGrp] = xGrpPos;

                    unusedGrps.pop_back();
                    unusedPos[xGrp] = -1;
                }


                for (size_t i = 1; i < r1Grps.size(); i++)
                {
                    size_t grpIdx = r1Grps[i];
                    int& grpPos = unusedPos[grpIdx];
                    if (grpPos >= 0) continue;

                    // Add to list of unused groups.
                    grpPos = unusedGrps.size();
                    unusedGrps.push_back(grpIdx);
                }
            }


            // --- Line 9 ---

            while (unusedGrps.size() > 0)
            {
                // --- Lines 10 + 11 ---

                size_t cIdx = unusedGrps.back();
                int yId = P.firstInGroup(cIdx);
                pivot[cIdx] = yId;


                // --- Lines 12 + 13 ---

                vector<size_t> newGrps = P.r2Refine(yId, g[yId]);

                // Mark C as used.
                // Because we picked C as last group in unusedGrps, we can
                // simply drop it from the list.
                unusedGrps.pop_back();
                unusedPos[cIdx] = -1;

                // Mark new groups as unused.
                for (const size_t& grpIdx : newGrps)
                {
                    unusedPos[grpIdx] = unusedGrps.size();
                    unusedGrps.push_back(grpIdx);
                }
            }


            // --- Line 14 ---

            vector<size_t> zGrps = P.findLRNonSingles(oId);

            if (zGrps.size() == 0)
            {
                throw logic_error("Unable to find groups.");
            }

            // Remove group with old origin.
            P.dropIfSingle(oId);

            if (zGrps.size() == 1)
            {
                // Well, guess its pivot is the new origin.
                size_t zGrp = zGrps[0];
                oId = pivot[zGrp];
                continue;
            }


            // --- Line 15 ---

            size_t lGrp = zGrps[0];
            size_t rGrp = zGrps[1];

            int zL = pivot[lGrp];
            int zR = pivot[rGrp];


            // -- Check if zL and zR are adjacent. --

            int zSml = zL;
            int zLrg = zR;

            if (g[zL].size() > g[zR].size())
            {
                swap(zSml, zLrg);
            }

            const vector<int> zSNeig = g[zSml];
            bool adjacent = false;

            for (const int& nId : zSNeig)
            {
                if (nId == zLrg)
                {
                    adjacent = true;
                    break;
                }
            }

            oId = adjacent ? zL : zR;
        }

        return P.getOrder();
    }

    // Computes a cotree for the given graph if it is a cograph.
    // Otherwise nothing is returned.
    bool cotree(const Graph& g)
    {
        // ToDo: Return prper cotree.


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


        size_t n = g.size();


        // --- Line 0 ---

        vector<int> sigma = factPermutation(g);


        // --- Line 3 ---

        // Index of the preceeding element in sigma.
        // We use int instead of size_t, to have -1 as null-pointer.
        vector<int> pre(n);

        for (int i = 0; i < n; i++)
        {
            pre[i] = i - 1;
        }

        // States if a vertex was removed from sigma.
        vector<bool> removed(n, false);


        // --- Line 4 ---

        // We simplify the loop starting in line 4 as shown below.
        // The change avoids checking the same pair of vertices twice.

        // For z := v_1 To v_n
        //     While z and pre(z) are twins (true or false) in G(sigma)
        //         Remove pre(z) from sigma.

        for (int z = 1; z < n; z++)
        {
            // --- Lines 5 - 11 (modified) ---

            int zId = sigma[z];

            for (int& p = pre[z]; p >= 0;)
            {
                int pId = sigma[p];

                TwinType tt = checkTwins(g, zId, pId, removed);
                if (tt == TwinType::None) break;

                // z and pre(z) are twins.

                // "Remove" pre(z) from sigma.
                removed[pId] = true;
                p = pre[p]; // Also updates pre[z].
            }
        }


        // --- Lines 12 - 15 ---

        // The given graph is a cograph if and only if the imaginary linked list
        // contains only one entry. The list only contains one entry if and only
        // if the last entry has no preceeding element.

        return pre[n - 1] < 0;
    }
}
