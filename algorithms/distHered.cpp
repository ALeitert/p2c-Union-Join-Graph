#include "distHered.h"


#include <cassert>

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
        // Follow terminology from [2].
        typedef Group Part;


    public:

        // Default constructor.
        // Creates an empty data structure.
        CographPR() : PartRefinement() { }

        // Constructor.
        // Creates a partition refinement of size n.
        CographPR(size_t n) : PartRefinement(n)
        {
            unusedPos.resize(n, -1);
            pivot.resize(n, -1);

            // The initial part (contining everything) is unused.
            unusedParts.push_back(0);
            unusedPos[0] = 0;
        }


        // Refines the part containing the given vertex x according to Rule 1
        // for factorising permutations.
        void r1Refine(int xId, const vector<int>& xNeigh)
        {
            // -- Rule 1 Refinement --

            // For a given part C and vertex x in C, split C into
            //     [ co-N(x) \cap C, { x }, N(x) \cap C ].

            // Make co-N(x) \cap C and N(x) \cap C unused.
            // Make { x } used with x as pivot.


            // Determine part C.
            size_t cIdx = id2Grp[xId];
            Part& C = groups[cIdx];

            // Determine neighbours of x in C.
            vector<int> neiList;
            for (const int& id : xNeigh)
            {
                size_t prtIdx = id2Grp[id];
                if (prtIdx == cIdx) neiList.push_back(id);
            }

            // Has x neighbours in C?
            if (neiList.size() > 0)
            {
                // Yes. Refine() adds them into a new part which follows C.
                refine(neiList);
                makeUnused(C.next);
            }

            // Is x alone in C?
            if (C.start < C.end)
            {
                // No. Refine() adds it into a new part which follows C.
                refine(vector<int> { xId });

                makeUnused(cIdx);
                makeUsed(C.next, xId);
            }
            else
            {
                // Yes. Mark C = { x } as used.
                makeUsed(cIdx, xId);
            }
        }

        // Refines the current parts which do not contain the given vertex y
        // according to Rule 2 for factorising permutations.
        void r2Refine(int yId, const vector<int>& yNeigh)
        {
            // -- Rule 2 Refinement --
            // See Procedure 4 in [2].

            // For a given vertex y, split all parts X not containing y into
            //     [ co-N(y) \cap X, N(y) \cap X ].

            // If X was unused, then mark both parts as unused too.
            // If X was used, mark the part containing its pivot as used and
            // the other part as unused.

            // Mark the part C containing y as used.
            // If C is a singleton part, drop it.


            // --- Process C. ---

            size_t cIdx = id2Grp[yId];
            makeUsed(cIdx, yId);

            bool isSingle = dropIfSingle(yId);


            // --- Determine neighbours of y not in C and refine them. ---

            vector<int> newNeigh;

            if (!isSingle)
            {
                for (const int& id : yNeigh)
                {
                    size_t prtIdx = id2Grp[id];
                    if (prtIdx != cIdx) newNeigh.push_back(id);
                }
            }

            vector<size_t> newParts = refine(isSingle ? yNeigh : newNeigh);


            // --- Mark new and refined parts according to Rule 2. ---

            // If a part X is a subset of yNeigh, then Refine() did not change
            // it and did not create a new part after it. Subsequently, X is not
            // listed in newParts[].

            for (size_t xaIdx : newParts)
            {
                // Part X was split into X and Xa.
                // Xa contains the neighbours of y and succeds X.

                size_t xIdx = groups[xaIdx].prev;

                // Was X unused before Refine()?
                if (unusedPos[xIdx] >= 0)
                {
                    // Yes. Mark Xa as unused too.
                    makeUnused(xaIdx);
                }
                else
                {
                    // No. Determine pivot and which part it contains.

                    int xPiv = pivot[xIdx];

                    if (id2Grp[xPiv] == xIdx)
                    {
                        // X remains used with pivot x.
                        makeUnused(xaIdx);
                    }
                    else
                    {
                        makeUnused(xIdx);
                        makeUsed(xaIdx, xPiv);
                    }
                }
            }
        }


        // Determines if the data structure contains unused parts.
        // If so, picks an element from an unused part, writes it in the given
        // ID, and returns true.
        // Otherwise, returns false and does not change the given ID.
        bool findUnusedPivot(int& id)
        {
            if (unusedParts.size() == 0) return false;

            size_t prtIdx = unusedParts.back();
            const Part& part = groups[prtIdx];

            id = order[part.start];
            return true;
        }

        // Determines the nearest non-singleton parts to the left and right of
        // the part containing the given element.
        // Returns the pivots of these parts.
        vector<int> findLRPivots(int id)
        {
            vector<int> result;

            size_t prtIdx = id2Grp[id];
            const Part& part = groups[prtIdx];

            // Search on left.
            for (size_t lIdx = part.prev; lIdx < groups.size(); lIdx = groups[lIdx].prev)
            {
                const Part& lPart = groups[lIdx];

                if (lPart.start < lPart.end)
                {
                    // Part has more than one entry.
                    result.push_back(pivot[lIdx]);
                    break;
                }
            }

            // Search on right.
            for (size_t rIdx = part.next; rIdx < groups.size(); rIdx = groups[rIdx].next)
            {
                const Part& rPart = groups[rIdx];

                if (rPart.start < rPart.end)
                {
                    // Part has more than one entry.
                    result.push_back(pivot[rIdx]);
                    break;
                }
            }

            return result;
        }


        // Drops the part containing the given ID if it is a singleton part.
        // Returns false if the given ID is not in a singleton.
        bool dropIfSingle(int id)
        {
            assert(id >= 0 && id < id2Grp.size());

            size_t prtIdx = id2Grp[id];
            if (prtIdx == -1) return true;

            const Part& part = groups[prtIdx];
            if (part.start < part.end) return false;


            // ID is only element in part.
            // Remove part from list.

            size_t pIdx = part.prev;
            size_t nIdx = part.next;

            if (pIdx != -1) groups[pIdx].next = nIdx;
            if (nIdx != -1) groups[nIdx].prev = pIdx;

            if (fGrpIdx == prtIdx) fGrpIdx = nIdx;
            if (lGrpIdx == prtIdx) lGrpIdx = pIdx;

            grpCount--;
            id2Grp[id] = -1;

            return true;
        }


    private:

        // All unused parts (in no particular order).
        vector<size_t> unusedParts;

        // States the index of a part in the list of used parts.
        // Unused parts have position -1.
        vector<int> unusedPos;

        // The pivot of each part.
        vector<int> pivot;

        // Marks the given part as used and assigns it the given pivot.
        void makeUsed(size_t prtIdx, int piv)
        {
            int pos = unusedPos[prtIdx];
            if (pos >= 0)
            {
                // Swap with last entry.
                swap(unusedParts[pos], unusedParts.back());
                size_t lastGrp = unusedParts[pos];
                unusedPos[lastGrp] = pos;

                unusedParts.pop_back();
                unusedPos[prtIdx] = -1;
            }

            pivot[prtIdx] = piv;
        }

        // Marks the given part as unused.
        void makeUnused(size_t prtIdx)
        {
            pivot[prtIdx] = -1;
            int pos = unusedPos[prtIdx];
            if (pos >= 0) return;

            unusedPos[prtIdx] = unusedParts.size();
            unusedParts.push_back(prtIdx);
        }
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

        CographPR P(n);


        // --- Line 2 ---

        int oId = 0;


        // --- Line 5 ---

        while (P.dropSingles())
        {
            // --- Line 6 ---

            if (!P.isDroppedOrSingle(oId))
            {
                // --- Line 7 + 8 ---

                P.r1Refine(oId, g[oId]);
            }


            // --- Line 9 - 13 ---

            for (int yId = -1; P.findUnusedPivot(yId);)
            {
                P.r2Refine(yId, g[yId]);
            }


            // --- Line 14 ---

            vector<int> zPivots = P.findLRPivots(oId);

            // Remove part with current origin.
            P.dropIfSingle(oId);


            // --- Line 15 - 18 ---


            // No more non-singleton parts?
            if (zPivots.size() == 0) break;

            // Only one side has a non-sigleton part?
            if (zPivots.size() == 1)
            {
                oId = zPivots[0];
                continue;
            }


            // -- There is a non-singlton part on either side of the origin. --

            int zL = zPivots[0];
            int zR = zPivots[1];


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


    // Represents a cotree.
    class Cotree
    {
    public:

        // Represents the different types of nodes.
        enum class CotreeNode
        {
            // A vertex of the original graph.
            Vertex,

            // Combines two subgraphs without adding edges.
            Union,

            // Combines two subgraphs and adds all possible edges from vertices
            // int the first to vertices in the second.
            Join
        };


    private:

        // The ID of the root.
        int root = -1;

        // The parent of each node.
        vector<int> parents;

        // The type of each node.
        vector<CotreeNode> nodeTypes;

        // The adjacency list of the tree.
        vector<vector<int>> adjList;
    };

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
