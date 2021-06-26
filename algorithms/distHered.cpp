#include "distHered.h"


#include <cassert>

#include "../dataStructures/partRefine.h"
#include "../dataStructures/unionFind.h"


// --- Papers ---

// [1] Damiand, Habib, Paul:
//     A simple paradigm for graph recognition: application to cographs and distance hereditary graphs.
//     Theoretical Computer Science 263, 99 - 111, 2001.

// [2] Habib, Paul:
//     A simple linear time algorithm for cograph recognition.
//     Discrete Applied Mathematics 145, 183 - 197, 2005.


// Anonymous namespace for "helper" functions.
// Implements recognition of cographs according to [2].
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


        // Default constructor.
        // Creates an empty tree.
        Cotree() { /* Nothing to do. */ }

        // Constructor.
        // Creates a "tree" with n vertex-nodes.
        // Does not create any edgesl
        Cotree(size_t n)
        {
            parents.resize(n, -1);
            nodeTypes.resize(n, CotreeNode::Vertex);
            children.resize(n);
            localRoot.resize(n);

            // Each node is its own root.
            for (int id = 0; id < n; id++)
            {
                localRoot[id] = id;
            }
        }


        // Returns the parents of all nodes.
        const vector<int>& getParents() const
        {
            return parents;
        }

        // Returns what type a given node is.
        CotreeNode operator()(int nodeId)
        {
            assert(nodeId >= 0 && nodeId < nodeTypes.size());
            return nodeTypes[nodeId];
        }


        // Combines the subrtrees of two vertices with a node of the given type.
        void mergeSubtrees(int xId, int yId, CotreeNode rType)
        {
            assert(xId >= 0 && xId < localRoot.size());
            assert(yId >= 0 && yId < localRoot.size());
            assert(rType == CotreeNode::Join || rType == CotreeNode::Union);


            // --- Determine old roots. ---

            int xRoot = localRoot[xId];
            int yRoot = localRoot[yId];

            if (xRoot == yRoot) return;


            // --- Create new root. ---

            int newRoot = parents.size();

            // Add to tree.
            parents.push_back(-1);
            nodeTypes.push_back(rType);
            children.push_back(vector<int>());

            // Make new root parent of old roots.
            children[newRoot].push_back(xRoot);
            children[newRoot].push_back(yRoot);

            parents[xRoot] = newRoot;
            parents[yRoot] = newRoot;

            // Update x and y.
            localRoot[xId] = newRoot;
            localRoot[yId] = newRoot;
        }

        // States if the tree is empty or not.
        bool isEmpty() const
        {
            return parents.size() == 0;
        }

        // Computes a post-order of the nodes in the tree.
        vector<int> getPostOrder() const
        {
            size_t n = parents.size();
            vector<bool> used(n, false);


            // --- Prepare DFS. ---

            vector<int> postOrder(n);
            size_t postIdx = 0;

            // Helpers to compute DFS.
            vector<size_t> childIndex;
            childIndex.resize(n, 0);

            vector<int> stack;


            // --- Run DFS. ---

            for (int nodeId = 0; nodeId < n; nodeId++)
            {
                if (used[nodeId]) continue;

                int root = nodeId;
                while (parents[root] >= 0) root = parents[root];

                stack.push_back(root);

                while (stack.size() > 0)
                {
                    int nId = stack.back();
                    size_t cIdx = childIndex[nId];

                    if (cIdx == 0)
                    {
                        // *** Pre-order for nId ***
                        used[nId] = true;
                    }

                    if (cIdx < children[nId].size())
                    {
                        int cId = children[nId][cIdx];

                        // No need to check if child was visited before,
                        // since we only have edges to children.

                        stack.push_back(cId);
                        childIndex[nId]++;
                    }
                    else
                    {
                        // All neighbours checked, backtrack.
                        stack.pop_back();

                        // *** Post-order for vId ***
                        postOrder[postIdx] = nId;
                        postIdx++;
                    }
                }
            }

            return postOrder;
        }


    private:

        // The parent of each node.
        vector<int> parents;

        // The type of each node.
        vector<CotreeNode> nodeTypes;

        // The adjacency list of the tree.
        vector<vector<int>> children;

        // States root-ID of the current subtree of a given vertex.
        vector<int> localRoot;
    };

    // Computes a cotree for the given graph if it is a cograph.
    // Otherwise nothing is returned.
    Cotree cotree(const Graph& g)
    {
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
        Cotree coT(n);


        // --- Line 0 ---

        vector<int> sigma = factPermutation(g);


        // --- Line 3 ---

        // Index of the preceeding element in sigma.
        vector<size_t> pre(n);

        for (size_t i = 0; i < n; i++)
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

        for (size_t z = 1; z < n; z++)
        {
            // --- Lines 5 - 11 (modified) ---

            int zId = sigma[z];

            for (size_t& p = pre[z]; p < z;)
            {
                int pId = sigma[p];

                TwinType tt = checkTwins(g, zId, pId, removed);
                if (tt == TwinType::None) break;

                // z and pre(z) are twins.

                // Update cotree.
                coT.mergeSubtrees
                (
                    zId,
                    pId,
                    tt == TwinType::FalseTwin
                    ?
                        Cotree::CotreeNode::Union
                    :
                        Cotree::CotreeNode::Join
                );

                // "Remove" pre(z) from sigma.
                removed[pId] = true;
                p = pre[p]; // Also updates pre[z].
            }
        }


        // --- Lines 12 - 15 ---

        // The given graph is a cograph if and only if the imaginary linked list
        // contains only one entry. The list only contains one entry if and only
        // if the last entry has no preceeding element.

        return pre[n - 1] >= n - 1 ? coT : Cotree();
    }
}


// Computes a pruning sequence for a given cograph.
// Returns an empty list if the given graph is not a cograph.
vector<DistH::Pruning> DistH::pruneCograph(const Graph& g)
{
    // --- Algorithm 2 from [1] ---

    // Note that the algorithm produces an elimination order.

    //  1  Compute a cotree T of G.
    //  2  Let A be the nodes of T having only leaves as descendant.
    //  3  While A is non-empty
    //  4      Pick an arbitrary node N in A.
    //  5      Pick an arbitrary child x of N.
    //  6      For Each child y != x of N
    //  7          If N is a 1-node, set s_j := (yTx). Otherwise, set s_j := (yFx).
    //  8          Set j := j + 1.
    //  9      Replace N by x in T.
    // 10      If x is the root of T Then
    // 11          Stop: x is the last vertex of the pruning sequence.
    // 12      If parent(x) has only leaves as descendant, add parent(x) into A.

    // As mentioned in [1], the algorithm above can be implemented with a
    // post-order. We use that apporach and change the algorithm as follows.

    //  2  Find a post-order < x_1, ..., x_m > of T.
    //  3  Create an empty stack S.
    //  4  For i := 0 To m - 1
    //  5      Let p = parent(x_i).
    //  6      If x_i is last child of parent (i.e., x_{i + 1} = p) Then
    //  7          While parent(S.top()) = p
    //  8              Pop S and let y be the removed vertex.
    //  9              If p is a 1-node, set s_i := (yTx_i).
    //                 Otherwise, set s_i := (yFx_i).
    // 10              Set parent(x_i) := parent(x_{i + 1}) and x_{i + 1} := x_i.
    // 11      Else
    // 12          Push x_i onto S.


    // --- Line 1 ---

    Cotree coT = cotree(g);
    if (coT.isEmpty()) return vector<Pruning>();


    // --- Lines 2 + 3 ---

    vector<int> postOrder = coT.getPostOrder();
    vector<int> parents = coT.getParents();

    vector<int> stack;

    vector<Pruning> result;


    // --- Line 4 ---

    for (size_t i = 0; i < postOrder.size() - 1; i++)
    {
        // --- Line 5 ---

        int xId = postOrder[i];
        int& xPar = parents[xId];
        int& next = postOrder[i + 1];


        // --- Line 6 ---

        if (next == xPar)
        {
            // x is last child of parent.

            PruningType pType =
            (
                coT(xPar) == Cotree::CotreeNode::Union
                ?
                    // 0-Node
                    PruningType::FalseTwin
                :
                    // 1-Node
                    PruningType::TrueTwin
            );


            // --- Lines 7 - 9---

            for (; stack.size() > 0; stack.pop_back())
            {
                int yId = stack.back();
                if (parents[yId] != xPar) break;

                result.push_back(Pruning(yId, pType, xId));
            }


            // --- Line 10 ---

            // xPar and next are reference.
            xPar = parents[xPar];
            next = xId;
        }
        else
        {
            // --- Line 12 ---

            stack.push_back(xId);
        }
    }


    // --- Add "last" vertex. ---

    int vId = postOrder.back();
    result.push_back(Pruning(vId, PruningType::Pendant, -1));


    return result;
}

// Computes a pruning sequence for a given cograph.
// Returns an empty list if the given graph is not a cograph.
vector<DistH::Pruning> DistH::pruneCograph_noTree(const Graph& g)
{
    // The algorithm from [1] is based on a cotree. We can skip that step and
    // combine the algorithm to build a cotree with the algrithm to compute a
    // pruning sequence from such a tree.


    size_t n = g.size();

    vector<int> sigma = factPermutation(g);

    // Index of the preceeding element in sigma.
    vector<size_t> pre(n);
    for (size_t i = 0; i < n; i++)
    {
        pre[i] = i - 1;
    }

    // States if a vertex was removed from sigma.
    vector<bool> removed(n, false);


    // --- Process permutation. ---

    vector<DistH::Pruning> result;

    for (size_t z = 1; z < n; z++)
    {
        int zId = sigma[z];

        for (size_t& p = pre[z]; p < z;)
        {
            int pId = sigma[p];

            TwinType tt = checkTwins(g, zId, pId, removed);
            if (tt == TwinType::None) break;

            // z and pre(z) are twins.

            DistH::PruningType pType =
            (
                tt == TwinType::TrueTwin
                ?
                    // 1-Node
                    DistH::PruningType::TrueTwin
                :
                    // 0-Node
                    DistH::PruningType::FalseTwin
            );

            result.push_back(DistH::Pruning(pId, pType, zId));


            // "Remove" pre(z) from sigma.
            removed[pId] = true;
            p = pre[p]; // Also updates pre[z].
        }
    }


    // --- Add "last" vertex. ---

    int vId = sigma.back();
    result.push_back(Pruning(vId, PruningType::Pendant, -1));


    // The given graph is a cograph if and only if the imaginary linked list
    // contains only one entry. The list only contains one entry if and only
    // if the last entry has no preceeding element.

    return pre[n - 1] >= n - 1 ? result : vector<DistH::Pruning>();
}


// Anonymous namespace with helper functions for pruneDistHered().
namespace
{
    // Computes distance layers of a given graph.
    vector<vector<int>> bfs(const Graph& g, int sId)
    {
        size_t n = g.size();


        // --- Initialise search. ---

        vector<vector<int>> Q;
        vector<bool> inQueue(n, false);

        // Set start vertex
        Q.push_back(vector<int> { sId });
        inQueue[sId] = true;

        // --- Run BFS. ---

        for (size_t dist = 0; Q.back().size() > 0; dist++)
        {
            // Ensure there is a queue for the next layer.
            Q.push_back(vector<int>());

            vector<int>& currQ = Q[dist];
            vector<int>& nextQ = Q[dist + 1];

            for (size_t qIdx = 0; qIdx < currQ.size(); qIdx++)
            {
                int vId = currQ[qIdx];
                const vector<int>& vNeighs = g[vId];

                for (const int& uId : vNeighs)
                {
                    if (inQueue[uId]) continue;

                    nextQ.push_back(uId);
                    inQueue[uId] = true;
                }
            }
        }

        // Remove last empty queue.
        Q.pop_back();

        return Q;
    }

    // Determine the number of lower neighbours for each vertex.
    vector<size_t> getInnerDegree(const Graph& g, const vector<size_t>& id2Layer)
    {
        const size_t n = g.size();
        vector<size_t> innerDegree(n, 0);

        for (int vId = 0; vId < n; vId++)
        {
            size_t vLayer = id2Layer[vId];
            size_t& vCount = innerDegree[vId];

            for (const int& uId : g[vId])
            {
                // Neighbour is in a lower layer?
                if (id2Layer[uId] < vLayer) vCount++;
            }
        }

        return innerDegree;
    }

    // Sorts vertices within layers by inner degree of vertices.
    // Does not change the given layers and returns a new list of layers instead.
    vector<vector<int>> sortByDegree
    (
        const Graph& g,
        const vector<size_t>& id2Layer,
        const vector<size_t>& innerDegree,
        const size_t k /* the total number of layers */
    )
    {
        const size_t n = g.size();


        // --- Counting sort. ---

        // Result of sorting.
        vector<int> byDegree(n);

        // Counter for counting sort.
        vector<size_t> counter(n, 0);


        // Count.
        for (int vId = 0; vId < n; vId++)
        {
            size_t key = innerDegree[vId];
            counter[key]++;
        }

        // Pre-fix sums.
        for (size_t i = 1; i < n; i++)
        {
            counter[i] += counter[i - 1];
        }

        // Sort.
        for (int vId = n - 1; vId >= 0; vId--)
        {
            size_t key = innerDegree[vId];

            counter[key]--;
            size_t idx = counter[key];

            byDegree[idx] = vId;
        }


        // -- Rebuild layers. --

        vector<vector<int>> layers(k);

        // Add vertices into layers again.
        for (size_t idx = 0; idx < n; idx++)
        {
            int vId = byDegree[idx];
            size_t vLayer = id2Layer[vId];
            layers[vLayer].push_back(vId);
        }

        return layers;
    }

    // Creates the subgraph from a given list of vertices.
    Graph createSubgraph
    (
        const Graph& g,
        const vector<int>& vList,
        const vector<int>& sgIds
    )
    {
        vector<intPair> edgeList;

        // Determine edges of subgraph.
        for (size_t vIdx = 1; vIdx < vList.size(); vIdx++)
        {
            int vId = vList[vIdx];
            const vector<int>& vNeighs = g[vId];

            for (size_t uIdx = 0; uIdx < vNeighs.size(); uIdx++)
            {
                int uId = vNeighs[uIdx];

                // The edges need to go from larger to smaller vertex.
                if (uId > vId) break;

                if (sgIds[uId] >= 0)
                {
                    edgeList.push_back(intPair(vIdx, sgIds[uId]));
                }
            }
        }

        // Add last vertex with "edge" to itself. That ensures that the graph
        // has the correct number of vertices even if they are isolated.
        int lastSgId = vList.size() - 1;
        edgeList.push_back(intPair(lastSgId, lastSgId));

        return Graph(edgeList, vector<int>(edgeList.size()));
    }

    // Contracts a subgraph defined by the given vertices and add its pruning
    // sequence to the given list.
    // Return the ID into which the subgraph is contracted.
    int contractSG
    (
        const Graph& g,
        const vector<int> vList,
        vector<int>& sgIds,
        vector<DistH::Pruning>& result
    )
    {
        // Check trival cases.
        // Needed to avoid errors.
        if (vList.size() == 0) return -1;
        if (vList.size() == 1) return vList[0];


        // --- Create subgraph and pruning sequence. ---

        // Set IDs for subgraph.
        for (int id = 0; id < vList.size(); id++)
        {
            int vId = vList[id];
            sgIds[vId] = id;
        }

        Graph sg = createSubgraph(g, vList, sgIds);
        vector<DistH::Pruning> sgPrune = DistH::pruneCograph_noTree(sg);

        // Reset IDs for subgraph.
        for (const int& vId : vList)
        {
            sgIds[vId] = -1;
        }


        // --- Process sequence (except last two). ---

        // Subgraph is cograph?
        if (sgPrune.size() < vList.size()) return -1;

        // Ignore last entry.
        sgPrune.pop_back();

        // Restore IDs of G.
        for (DistH::Pruning& prun : sgPrune)
        {
            prun.vertex = vList[prun.vertex];
            prun.parent = vList[prun.parent];
        }

        // Add all but last to overall result.
        for (size_t idx = 0; idx + 1 < sgPrune.size(); idx++)
        {
            DistH::Pruning& prun = sgPrune[idx];
            result.push_back(prun);
        }


        // --- Process last entry. ---

        // When creating a subgraph G[X] (with X = N_{i - 1}(u)), we process the
        // full neighbourhood of each x in X with respect to G, not just G[X].
        // X might contain a vertex y with a particular high number of
        // neighbours. If y is the vertex we contract X into, it can happen that
        // we use y over and over again to construct subgraphs. To ensure linear
        // runtime, we want to use each vertex at most once as result of a
        // contraction.

        // Note that in a pruning sequence, on can alway swap the last two
        // vertices. That allows us to decide which vertex we contract into and,
        // thereby, avoiding to have the same vertex twice.

        // That beeing said, we do slightly divert from that approach. Instead,
        // we pick the vertex with the smaller neighbourhood in G.

        DistH::Pruning& prun = sgPrune.back();

        int& xId = prun.vertex;
        int& pId = prun.parent;

        if (g[xId].size() < g[pId].size())
        {
            swap(xId, pId);
        }

        result.push_back(prun);


        return pId;
    }
}


// Computes a pruning sequence for a given distance-hereditary graph.
// Returns an empty list if the given graph is not distance-hereditary.
vector<DistH::Pruning> DistH::pruneDistHered(const Graph& g)
{
    // --- Algorithm 3 from [1] ---

    //  1  Set j := 1.
    //  2  Compute the distance layout < L_1, ..., L_k > from an arbitrary
    //     vertex v.
    //  3  For i := k DownTo 1
    //  4      For Each connected component CC of G[L_i]
    //  5          Set z := PruneCograph(G[CC], j).
    //  6          Contract CC into z.
    //  7          Set j := j + |CC| - 1.
    //  8      Sort the vertices of G[L_i] by increasing inner degree.
    //  9      For Each vertex x of L_i with inner degree 1
    // 10          Let y be the only neighbour of x.
    // 11          Set sigma(j) := x, s_j := (xPy), and j := j + 1.
    // 12      If i == 1, stop.
    // 13      For Each x in L_i taken in increasing inner degree order
    // 14          Set y := PruneCograph(G[N_{i - 1}(x)], j).
    // 15          Contract N_{i - 1}(x) into y.
    // 16          Set j := j + |N_{i - 1}(x)| - 1
    // 17          Set sigma(j) := x, s_j := (xPy), and j := j + 1.

    // Note on lines 8, 9, and 13: The "inner degree" of a vertex is the number
    // of neighbours the vertex has in the layer below in the original graph.


    const size_t n = g.size();


    // --- Line 2 ---

    const int startId = 0;
    vector<vector<int>> layers = bfs(g, startId);
    const size_t k = layers.size();

    // Determine the layer of each vertex.
    vector<size_t> id2Layer(n);
    for (size_t i = 0; i < k; i++)
    {
        for (const int& vId : layers[i])
        {
            id2Layer[vId] = i;
        }

        // Clear layer so we can rebuild it later.
        layers[i].clear();
    }

    // Rebuild layers to sort them.
    for (int vId = 0; vId < n; vId++)
    {
        size_t l = id2Layer[vId];
        layers[l].push_back(vId);
    }


    // --- Preprocessing for later. ---

    // States if a vertex was "removed" by contracting vertices.
    vector<bool> ignore(n, false);


    // Union find to determine connected components.
    UnionFind uf(n);

    // Determines for the representetive of a CC the index of that component.
    vector<size_t> ccRep2Idx(n, -1);

    // Used to construct subgraphs.
    vector<int> sgIds(n, -1);


    // Determine the inner degree of all vertices.
    vector<size_t> inDegree = getInnerDegree(g, id2Layer);

    // Sort vertices in layers by their number of neighbours below.
    vector<vector<int>> sortedLayers = sortByDegree(g, id2Layer, inDegree, k);


    // The resulting sequence.
    vector<Pruning> result;


    // --- Line 3 ---

    // We skip layer 0 and treat it later as special case.
    for (size_t i = k - 1, ccIdx; i > 0; i--)
    {
        const vector<int>& iLayer = layers[i];


        // --- Determine connected components for lines 4 and 5. ---

        // The connected components of layer i.
        vector<vector<int>> iCCList;

        for (const int& vId : iLayer)
        {
            if (ignore[vId]) continue;

            for (const int& uId : g[vId])
            {
                if (id2Layer[uId] != i) continue;
                uf.unionSets(uId, vId);
            }
        }

        // Processed in order to ensure CC is in order too.
        for (size_t idx = 0; idx < iLayer.size(); idx++)
        {
            const int& vId = iLayer[idx];
            if (ignore[vId]) continue;

            size_t ccRep = uf.findSet(vId);
            size_t& ccIdx = ccRep2Idx[ccRep];

            if (ccIdx >= n)
            {
                // Create new component.
                ccIdx = iCCList.size();
                iCCList.push_back(vector<int>());
            }

            iCCList[ccIdx].push_back(vId);
        }


        // --- Line 4 ---

        for (const vector<int>& cc : iCCList)
        {
            // --- Lines 5 - 7 ---

            int zId = contractSG(g, cc, sgIds, result);
            if (zId < 0) return vector<Pruning>();

            // "Remove" vertices from graph.
            for (const int& vId : cc)
            {
                ignore[vId] = vId != zId;
            }
        }


        // --- Line 8 ---

        // Vertices in layers are already sorted by degree.


        // --- Lines 9 and 13 ---

        const vector<int>& iLaySorted = sortedLayers[i];

        for (size_t degIdx = 0; degIdx < iLaySorted.size(); degIdx++)
        {
            int vId = iLaySorted[degIdx];
            if (ignore[vId]) continue;


            // --- Determine downwards neighbourhood. ---

            const vector<int>& vNeighs = g[vId];

            vector<int> vDownN;
            for (size_t uIdx = 0, id = 0; uIdx < vNeighs.size(); uIdx++)
            {
                int uId = vNeighs[uIdx];
                if (ignore[uId] || id2Layer[uId] >= i) continue;

                vDownN.push_back(uId);
            }


            // --- Lines 14 - 16 ---

            int yId = contractSG(g, vDownN, sgIds, result);
            if (yId < 0) return vector<Pruning>();

            // "Remove" vertices from graph.
            for (const int& uId : vDownN)
            {
                ignore[uId] = uId != yId;
            }

            // For distance-hereditary graphs, if x, y ∈ N_{i - 1}(u) are in
            // different connected components X and Y of L_{i - 1}, then
            // X ∪ Y ⊆ N(u)  [Bandelt, Mulder 1986].

            // It follows from the above that we contracted N_{i - 1}(u) into a
            // single vertex which has no neighbours in L_{i - 1}. Therefore, we
            // do not need to update connected components before processing the
            // next layer.


            // --- Lines 10, 11, and 17 ---

            result.push_back(Pruning(vId, PruningType::Pendant, yId));
            ignore[vId] = true; // No real need for it, but we do it just to be safe.
        }
    }


    // Layer 0: start vertex of BFS
    result.push_back(Pruning(startId, PruningType::Pendant, -1));

    return result;
}
