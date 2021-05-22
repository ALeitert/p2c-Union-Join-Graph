#include "../dataStructures/partRefine.h"
#include "alphaAcyclic.h"
#include "interval.h"
#include "sorting.h"


// Genrates an interval hypergraph with m edges and total size N.
Hypergraph Interval::genrate(size_t m, size_t N)
{
    // The algorithm is almost the same as for acyclic hypergraphs.
    // The difference is that the tree for interval hypergraphs is just a path.


    // --- Generate a random tree. ---

    // Random permutation.
    vector<int> edgeIds(m);
    Sorting::makePermutation(edgeIds.data(), m);


    // --- Determine size of each hyperedge. ---

    // At least one vertex in each hyperedge.
    vector<size_t> eSize(m, 1);

    // Randoly assign remaining vertices.
    for (size_t i = m; i < N; i++)
    {
        size_t eId = rand() % m;
        eSize[eId]++;
    }


    // --- Determine shared vertices between hyperedges and their parenst. ---

    // List of vertices in each hyperedge.
    vector<vector<int>> vLists(m);

    // Total number of vertices.
    size_t n = 0;

    // Vertices in the root.
    for (int rId = edgeIds[0]; n < eSize[rId]; n++)
    {
        vLists[rId].push_back(n);
    }

    // Remaining hyperedges.
    for (size_t i = 1; i < m; i++)
    {
        // Parent is preceding hyperedge.
        int eId = edgeIds[i];
        int pId = edgeIds[i - 1];

        size_t eS = eSize[eId];
        size_t pS = eSize[pId];

        vector<int>& pList = vLists[pId];
        vector<int>& eList = vLists[eId];

        // Number of shared vertices.
        size_t shared = rand() % min(eS, pS) + 1;
        Sorting::kShuffle(pList, shared);

        // Add shared vertices.
        for (size_t j = 0; j < shared; j++)
        {
            eList.push_back(pList[j]);
        }

        // Add remaining new vertices.
        for (size_t j = shared; j < eS; j++, n++)
        {
            eList.push_back(n);
        }
    }


    // --- Shuffle vertex IDs. ---

    vector<int> vIds;
    vIds.resize(n);
    Sorting::makePermutation(vIds.data(), n);

    for (size_t i = 0; i < m; i++)
    {
        vector<int>& lst = vLists[i];

        for (size_t j = 0; j < lst.size(); j++)
        {
            size_t idx = lst[j];
            lst[j] = vIds[idx];
        }
    }


    // --- Create hypergraph. ---

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
    vector<int>& vOrder = lexBfsOrd.second;

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
