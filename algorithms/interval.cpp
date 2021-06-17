#include "../dataStructures/partRefine.h"
#include "alphaAcyclic.h"
#include "interval.h"
#include "sorting.h"


// Anonymous namespace for helper functions.
namespace
{
    // Helper for LexBfs.
    typedef pair<vector<int>, vector<int>> idOrderPair;

    // Helper function to compute a LexBFS of the vertices in the given hypergraph.
    // Returns a list of all hyperedges and vertices in LexBFS order.
    idOrderPair lexBfs(const Hypergraph& hg)
    {
        // --- Algorithm 10 ---

        //  Input: A family of sets F.
        // Output: A LexBFS-Ordering of the vertices of F.

        //  1  Let L = (F).
        //  2  Set i := n.

        //  3  While L is not empty
        //  4      Let C be a clique in the right-most class in L.
        //  5      Pick an unnumbered vertex x from C.
        //  6      Set pi(x) := i.

        //  7      If all members of C are now numbered, then remove it from its class.
        //         If its class is now empty, then remove it from L.

        //  8      For Each class X_a in L
        //  9          Let Y be the members of X_a that contain x.
        // 10          If Y is not empty and Y != X_a, then remove Y from X_a and insert Y(?) to the right of X_a in L.



        const size_t n = hg.getVSize();
        const size_t m = hg.getESize();


        // --- Line 1 ---

        // Add all hyperedge-IDs into one list.
        PartRefinement L(m);

        // States the current position in each hyperedge.
        vector<size_t> edgeIdx;
        edgeIdx.resize(m, 0);


        // --- Line 2 ---

        vector<int> vLexOrder;
        vector<bool> inOrder(n, false);


        // --- Line 3 ---

        while (L.size() > 0)
        {
            // --- Line 4 ---

            // Hyperedge C in last class.
            int cId = L.last();
            const vector<int>& cEdge = hg[cId];


            // --- Line 5 ---

            // Find unnumbered vertex in C.
            size_t& xIdx = edgeIdx[cId];
            int xId = -1;
            for (; xIdx < cEdge.size(); xIdx++)
            {
                xId = cEdge[xIdx];
                if (!inOrder[xId]) break;
            }


            // --- Line 6 ---

            if (xIdx < cEdge.size())
            {
                // Found an unnumbered vertex.
                // Add it to LexBFS-order and "remove" it from C.

                inOrder[xId] = true;
                vLexOrder.push_back(xId);
                xIdx++;
            }


            // --- Line 7 ---

            // If all vertices in C are numbered, ...
            if (xIdx >= cEdge.size())
            {
                // ... then remove it from L.
                L.dropLast();
            }


            // --- Line 8 + 9 + 10 ---

            if (xId >= 0) L.refine(hg(xId));
        }

        return idOrderPair(L.getOrder(), vLexOrder);
    }

    // Computes the separator hypergraph for a given interval hypergraph with a given join path.
    // Returns a list of vertex-lists. The order corresponds to the order in the join path.
    vector<vector<int>> separators(const Hypergraph& hg, const vector<int>& joinPath)
    {
        const int m = hg.getESize();

        vector<vector<int>> sepHg(m - 1);

        for (size_t i = 0; i < sepHg.size(); i++)
        {
            int lId = joinPath[i];
            int rId = joinPath[i + 1];

            const vector<int>& lVert = hg[lId];
            const vector<int>& rVert = hg[rId];

            vector<int>& sep = sepHg[i];

            // Compute intersection.
            for (size_t l = 0, r = 0; l < lVert.size() && r < rVert.size();)
            {
                int lVId = lVert[l];
                int rVId = rVert[r];

                if (lVId <= rVId) l++;
                if (lVId >= rVId) r++;

                if (lVId == rVId) sep.push_back(lVId);
            }
        }

        return sepHg;
    }
}

// Genrates an interval hypergraph with m edges and total size N.
Hypergraph Interval::genrate(size_t m, size_t N)
{
    // List of vertices in each hyperedge.
    vector<vector<int>> vLists(m);

    size_t n = 0;

    // At least one vertex in each hyperedge.
    for (size_t e = m - 1, s; e > 0; n++, e = s)
    {
        s = rand() % e;

        for (size_t i = s; i <= e; i++)
        {
            vLists[i].push_back(n);
            N = min(N, N - 1);
        }
    }

    // Add remaining vertices randomly.
    for (; N > 0; n++)
    {
        size_t s = rand() % m;
        size_t e = rand() % (m - 1);

        // Ensures vertex is in at least two hyperedges.
        if (s <= e) e++;

        // Ensures s < e.
        if (s > e) swap(s, e);

        for (size_t i = s; i <= e; i++)
        {
            vLists[i].push_back(n);
            N = min(N, N - 1);
        }
    }


    // Build pair lists.
    vector<intPair> pairList;

    for (int eId = 0; eId < m; eId++)
    {
        vector<int>& lst = vLists[eId];

        for (size_t j = 0; j < lst.size(); j++)
        {
            int vId = lst[j];
            pairList.push_back(intPair(eId, vId));
        }
    }

    Sorting::radixSort(pairList);
    return Hypergraph(pairList);
}

// Computes a join path of a given hypergraph.
// Returns an order of hyperedges that is a valid join path.
// Returns an empty list if the hypergraph is not an interval hypergraph.
vector<int> Interval::getJoinPath(const Hypergraph& hg)
{
    // M. Habib, R. McConnell, C. Paul, L. Viennot:
    // Lex-BFS and partition refinement, with applicationsto transitive orientation, interval graph recognition and consecutive ones testing.
    // Theoretical Computer Science 234, 59-84, 2000.

    // --- Algorithm 9 ---

    //  Input: A graph G = (V, E)
    // Output: A clique chain L if G is interval.

    //  1  Compute the maximal cliques and a clique tree T = (X, F) using a LexBFS according to Algorithm 4.
    //  2  Let X = { C_1, C_2, ..., C_k } be the set of maximal cliques.
    //  3  Let L be the ordered list (X).
    //  4  Create an empty stack pivots = ∅.

    //  5  While there exists a non-singleton class X_C in L = ( X_1, X_2, ..., X_l )

    //  6      If pivots = ∅ Then
    //  7          Let C_l be the last clique in X_C discovered by the LexBFS (the clique with the greatest number).
    //  8          Replace X_C by X_C \ { C_l }, { C_l } in L.
    //  9          C = { C_l }
    // 10      Else
    // 11          Pick an unprocessed vertex x in pivots (throw away processed ones) and let C be the set of all maximal cliques containing x.
    // 12          Let X_a and X_b be the first and last classes containing a member of C.
    // 13          Replace X_a by X_a \ C, X_a \cap C and X_b by X_b \cap C, X_b \ C.

    // 14      For Each remaining tree edge (C_i, C_j) connecting a clique C_i in C to a clique C_j not in C
    // 15          Push C_i \cap C_j onto pivots.
    // 16          Remove (C_i, C_j) from the clique tree.

    // 17  For Each vertex x
    // 18      If the cliques containing x are not consecutive in the ordering Then
    // 19          Return "Not interval."
    // 20      Return "Interval."



    const size_t n = hg.getVSize();
    const size_t m = hg.getESize();

    if (m == 1) return vector<int> { -1 };


    // --- Lines 1 - 4 ---

    vector<int> joinTree = AlphaAcyclic::getJoinTree(hg);

    // Hypergraph acyclic?
    if (joinTree.size() == 0) return vector<int>();

    PartRefinement L(m);

    // Run LexBFS.
    idOrderPair lexBfsOrd = lexBfs(hg);
    vector<int>& eOrder = lexBfsOrd.first;

    // Stack for vertices.
    vector<int> pivot;

    // Processed vertices.
    vector<bool> processed(n, false);


    // --- Preprocessing for lines 14 - 16 ---

    // Allows to flag parts of the join tree.
    vector<bool> eFlags(m, false);

    // Convert to adjacency list.
    vector<vector<int>> tree;
    tree.resize(m);

    for (int eId = 0; eId < m; eId++)
    {
        int pId = joinTree[eId];
        if (pId == -1) continue;

        tree[eId].push_back(pId);
        tree[pId].push_back(eId);
    }



    // --- Line 5 ---

    for (auto eIt = eOrder.begin(); L.dropSingles();)
    {
        // -- Clear processed vertices from stack. --

        // We divert here from the given algorithm. The paper assumes that no
        // hyperedge is subset of or equal to another. The autors suggest to add
        // a dummy vertex into each hyperedge to ensure that. If two hyperedges
        // E_1 and E_2 are equal, it can happen that the stack (pivot) only
        // contains proccesd vertices while at the same time E_1 and E_2 are
        // still in the same group of the partition refinement. That causes an
        // exception when strictly following the given algorithm. To avoid that
        // problem, we remove processed vertices from the stack at the beginning
        // of each loop. As result, out implementation runs lines 7 to 9 instead
        // of lines 11 to 13.

        for (; pivot.size() > 0 && processed[pivot.back()]; pivot.pop_back()) { }


        // Set of hyperedges to process in lines 14 to 16.
        vector<int> C;


        // --- Line 6 ---

        if (pivot.size() == 0)
        {
            // --- Line 7 ---

            for (; eIt != eOrder.end() && L.isDroppedOrSingle(*eIt); ++eIt) { }
            int eId = *eIt;


            // --- Line 9 + 8 ---

            C.push_back(eId);
            L.refine(C);
        }
        else
        {
            // --- Line 11 ---

            // No need to processed vertices from stack here, since we do that
            // above at the beginning of the for-loop.

            int xId = pivot.back();
            processed[xId] = true;


            // --- Line 12 + 13 ---

            C = hg(xId);
            L.flRefine(C);
        }


        // --- Line 14 ---

        // Set a flag for all hyperedges in C. That way, we can quickly identify
        // if a hyperedge is in C or not.
        for (const int& eId : C) eFlags[eId] = true;

        // Iterate over all C_i in C.
        for (const int& eiId : C)
        {
            // Parent and neighbours of C_i.
            int& iPar = joinTree[eiId];
            vector<int>& neighs = tree[eiId];

            // Vertices in C_i.
            const vector<int>& iList = hg[eiId];

            // Iterate over all neighbous of C_i to find a C_j not in C.
            for (size_t nIdx = 0; nIdx < neighs.size(); nIdx++)
            {
                int ejId = neighs[nIdx];
                int& jPar = joinTree[ejId];

                // Edge still in tree?
                if (iPar != ejId && jPar != eiId)
                {
                    // No. Remove neighbour from list.
                    neighs[nIdx] = neighs.back();
                    neighs.pop_back();

                    // Next iteration.
                    nIdx--;
                    continue;
                }

                // If in C, skip.
                if (eFlags[ejId]) continue;


                // --- Line 15 ---

                // We found a C_j not in C.

                // Vertices in C_j.
                const vector<int>& jList = hg[ejId];

                // Add intersection of C_i and C_j onto stack (pivot).
                for (size_t i = 0, j = 0; i < iList.size() && j < jList.size();)
                {
                    int viId = iList[i];
                    int vjId = jList[j];

                    if (viId <= vjId) i++;
                    if (viId >= vjId) j++;

                    if (viId == vjId) pivot.push_back(viId);
                }


                // --- Line 16 ---

                // Remove tree edge C_iC_j ...

                // ... from list.
                neighs[nIdx] = neighs.back();
                neighs.pop_back();
                nIdx--;

                // ... from tree.
                if (iPar == ejId) iPar = -1;
                if (jPar == eiId) jPar = -1;
            }
        }

        // Reset a flag for all hyperedges in C.
        for (const int& eId : C) eFlags[eId] = false;
    }

    // We finished processingthe given hypergraph. If it is an interval
    // hypergraph, then the partition refinement produced an order of hyperedges
    // that is a valid join path for the hypergraph.
    const vector<int>& result = L.getOrder();


    // --- Check if acyclic. ---

    // We do not use the acyclicity test described in lines 17 to 20. Instead,
    // we use a simplified version of the test from the join tree algorithm.

    vector<size_t> vLastIdx(n, -1);

    // Iterate over sequence of hyperedges.
    for (int eIdx = 0; eIdx < result.size(); eIdx++)
    {
        int eId = result[eIdx];

        // Check and mark all vertices.
        for (const int& vId : hg[eId])
        {
            size_t& vIdx = vLastIdx[vId];

            if (vIdx == -1 || vIdx == eIdx - 1)
            {
                // Vertex is new or was in previous hyperedge.
                vIdx = eIdx;
            }
            else
            {
                // Order is not a valid interval order.
                return vector<int>();
            }
        }
    }

    // Hyperedge order satisfices acyclicity requirement.

    return result;
}

// Computes the edges of the subset graph of the given interval hypergraph.
// A pair (x, y) states that y is subset of x.
vector<intPair> Interval::subsetGraph(const Hypergraph& hg)
{
    vector<int> joinPath = getJoinPath(hg);

    if (joinPath.size() == 0)
    {
        throw invalid_argument("Given hypergraph is not interval.");
    }


    const size_t n = hg.getVSize();
    const size_t m = hg.getESize();


    // --- Determine range of each vertex. ---

    // The range of a vertex v is a pair of indices. They represent the first
    // and last hyperedge containing v with respect to the join path.

    vector<sizePair> vRange(n, sizePair(m, -1));

    for (size_t i = 0; i < m; i++)
    {
        int eId = joinPath[i];
        const vector<int>& vList = hg[eId];

        for (const int& vId : vList)
        {
            size_t& fr = vRange[vId].first;
            size_t& to = vRange[vId].second;

            fr = min(fr, i);
            to = i /* max(to, i) is always i */;
        }
    }


    // --- Determine subset relations. ---

    vector<intPair> result;

    for (size_t i = 0; i < m; i++)
    {
        int eId = joinPath[i];
        const vector<int>& vList = hg[eId];

        // The range that contains all vertices of e.
        size_t eFr = 0;
        size_t eTo = m;

        for (const int& vId : vList)
        {
            const size_t& vFr = vRange[vId].first;
            const size_t& vTo = vRange[vId].second;

            eFr = max(eFr, vFr);
            eTo = min(eTo, vTo);
        }

        // Larger hyperedges to the left.
        for (size_t j = eFr; j < i; j++)
        {
            int fId = joinPath[j];
            result.push_back(intPair(fId, eId));
        }

        // Larger hyperedges to the right.
        for (size_t j = i + 1; j <= eTo; j++)
        {
            int fId = joinPath[j];
            result.push_back(intPair(fId, eId));
        }
    }

    Sorting::radixSort(result);
    return result;
}

// Computes the union join graph for a given interval hypergraph.
// A slightly optimised function that avoids some overhead.
Graph Interval::unionJoinGraph(const Hypergraph& hg)
{
    // --- Determine join path and test if input is interval hypergraph. ---

    vector<int> joinPath = getJoinPath(hg);

    if (joinPath.size() == 0)
    {
        throw invalid_argument("Given hypergraph is not interval.");
    }


    // ---

    const size_t n = hg.getVSize();
    const size_t m = hg.getESize();

    vector<vector<int>> sepList = separators(hg, joinPath);


    // --- Determine range of each vertex. ---

    // The range of a vertex v is a pair of indices. They represent the first
    // and last hyperedge containing v with respect to the join path.

    vector<sizePair> vRange(n, sizePair(m, -1));

    for (size_t i = 0; i < m; i++)
    {
        int eId = joinPath[i];
        const vector<int>& vList = hg[eId];

        for (const int& vId : vList)
        {
            size_t& fr = vRange[vId].first;
            size_t& to = vRange[vId].second;

            fr = min(fr, i);
            to = i /* max(to, i) is always i */;
        }
    }


    // --- Determine range of each separator. ---

    vector<sizePair> sepRange(m, sizePair(0, m));

    for (size_t i = 0; i < m - 1; i++)
    {
        const vector<int>& vList = sepList[i];

        size_t& eFr = sepRange[i].first;
        size_t& eTo = sepRange[i].second;

        for (const int& vId : vList)
        {
            const size_t& vFr = vRange[vId].first;
            const size_t& vTo = vRange[vId].second;

            eFr = max(eFr, vFr);
            eTo = min(eTo, vTo);
        }
    }


    // --- Determine join tree (i.e. parents). ---

    vector<int> joinTree(m, -1);
    for (size_t i = 0; i < m - 1; i++)
    {
        int eId = joinPath[i + 1];
        size_t pIdx = sepRange[i].first;
        joinTree[eId] = joinPath[pIdx];
    }


    // --- Preprocessing for lines 6 and 7. ---

    // To determine which hyperedges are farther away and on which side of a
    // separator they are, we compute pre- and post-order of the join tree.

    AlphaAcyclic::orderPair jtDfs = AlphaAcyclic::joinTreeDfs(joinTree, joinPath[0]);

    vector<size_t>& pre = jtDfs.first;
    vector<size_t>& post = jtDfs.second;


    // --- Line 3: Create empty union join graph. ---

    vector<intPair> eList;
    vector<int> wList; // Will all be 0.


    // --- Line 4: Loop over all separators S. ---

    for (size_t i = 0; i < m - 1; i++)
    {
        int sChId = joinPath[i + 1];

        // --- Line 5: Determine all S' with S ⊆ S' (including S). ---

        const sizePair& range = sepRange[i];


        // --- Line 6: For each, S' determine hyperedges it represents. ---
        //     Line 7: Partition then based on their side of S in T.


        // All hyperedges that are below S, i.e., descendants of S in T.
        vector<int> downList; // bbE_1 in paper.

        // All hyperedges that are above S, i.e., not descendants of S in T.
        vector<int> aboveList; // bbE_2 in paper.


        for (size_t j = range.first; j < range.second; j++)
        {
            // Hyperedge below S'.
            // The way the sparator hypergraph is created, each separator has
            // the same ID as the hyperedge below it in the rooted join tree.
            int chiId = joinPath[j + 1];

            // Hyperedge above S'.
            int parId = joinTree[chiId];


            // ----------------------------------
            // From proof in paper:

            // x is a descendant of y if and only if
            // pre(x) > pre(y) and post(x) < post(y).

            // There are four cases when determining which of E and E' to pick:
            // 1) If S and S' represent the same edge, add E and E'.
            // 2) If S' is a descendant of S, add the child-hyperedge.
            // 3) If S' is an ancestor of S, add the parent-hyperedge.
            // 4) If S' is neither an ancestor nor a descendant of S,
            //    add the child-hyperedge.

            // Clearly, one side of S contains all its descendants and the other
            // side all remaining hyperedges and separators.

            // ----------------------------------


            // -- Determine ancestor/descendant relationship. --

            // We use the ID of the hyperedge below S and S' (which give S and
            // S' their IDs). If we would compute a pre- or post-order of T
            // where we treat separators as their own nodes, they would be
            // directly before (in pre-) or after (in post-) their corresponding
            // hyperedge. Thus, we still determine ancestory correctly.

            bool sIsDec = pre[sChId] > pre[chiId] && post[sChId] < post[chiId];
            bool sIsAnc = pre[chiId] > pre[sChId] && post[chiId] < post[sChId];


            // -- Add hyperedges. --

            // Case 1.
            if (chiId == sChId)
            {
                downList.push_back(chiId);
                aboveList.push_back(parId);
            }

            // Case 2.
            else if (sIsAnc)
            {
                // S' is a descendent of S.
                // Hence, the hyperedge below S' is farther from and below S.
                downList.push_back(chiId);
            }

            // Case 3.
            // Never happens. Assume we have case 3. All vertices in S would be
            // in the hyperedge above S'. Thus, the join tree algorithm sets
            // the hyperedge above S' as parent of the hyperedge below S.
            else if (sIsDec)
            {
                // S' is an ancestor of S.
                // Hence, the hyperedge above S' is farther from and above S.
                aboveList.push_back(parId);
            }

            // Case 4.
            else
            {
                // S' is neither an ancestor nor a descendant of S.
                // Hence, the hyperedge below S' is farther from and above S.
                aboveList.push_back(chiId);
            }
        }


        // --- Line 8: Add all E_1E_2 pairs. ---

        for (const int& e1 : downList)
        {
            for (const int& e2 : aboveList)
            {
                // Ensure that from > to.
                int fId = max(e1, e2);
                int tId = min(e1, e2);

                eList.push_back(sizePair(fId, tId));
            }
        }
    }

    Sorting::radixSort(eList);

    // Remove duplicates.
    if (eList.size() > 1)
    {
        size_t preIdx = 0;

        for (size_t i = 1; i < eList.size(); i++)
        {
            intPair prev = eList[preIdx];
            intPair curr = eList[i];

            if (prev == curr) continue;

            preIdx++;
            eList[preIdx] = curr;
        }

        eList.resize(preIdx + 1);
    }

    wList.resize(eList.size(), 0);

    return Graph(eList, wList);
}
