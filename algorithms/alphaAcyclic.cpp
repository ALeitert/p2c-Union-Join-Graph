#include "../dataStructures/maxCardinalitySet.h"
#include "../dataStructures/reducedSet.h"
#include "alphaAcyclic.h"
#include "sorting.h"


// Genrates an alpha-acyclic hypergraph with m edges and total size N.
Hypergraph AlphaAcyclic::genrate(size_t m, size_t N)
{
    // --- Generate a random tree. ---

    // Random permutation.
    int edgeIds[m];
    Sorting::makePermutation(edgeIds, m);

    // Set parents.
    int rootId = edgeIds[0];
    int parIds[m];
    parIds[rootId] = -1;

    for (size_t i = 1; i < m; i++)
    {
        int eId = edgeIds[i];

        int pIdx = rand() % i;
        int pId = edgeIds[pIdx];

        parIds[eId] = pId;
    }


    // --- Determine size of each hyperedge. ---

    size_t eSize[m];

    // At least one vertex in each hyperedge.
    for (size_t i = 0; i < m; i++)
    {
        eSize[i] = 1;
    }

    // Randoly assign remaining vertices.
    for (size_t i = m; i < N; i++)
    {
        size_t eId = rand() % m;
        eSize[eId]++;
    }


    // --- Determine shared vertices between hyperedges and their parenst. ---

    // List of vertices in each hyperedge.
    vector<int> vLists[m];

    // Total number of vertices.
    size_t n = 0;

    // Vertices in the root.
    for (; n < eSize[rootId]; n++)
    {
        vLists[rootId].push_back(n);
    }

    // Remaining hyperedges.
    for (size_t i = 1; i < m; i++)
    {
        int eId = edgeIds[i]; // current hyperedge
        int pId = parIds[eId]; // parent

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

    int vIds[n];
    Sorting::makePermutation(vIds, n);

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

// Computes a join tree of a given hypergraph.
// Returns a list that contains the parent-ID for each hyperedge.
// Returns an empty list if the given hypergraph is not acyclic.
vector<int> AlphaAcyclic::getJoinTree(const Hypergraph& hg)
{
    // Algorithm based on
    // R.E. Tarjan, M. Yannakakis:
    // Simple Linear-Time Algorithms to Test Chordality of Graphs, Test Acyclicity of Hypergraphs, and Selectively Reduce Acyclic Hypergraphs
    // SIAM J. Comput. 13 (3), 566–579, 1984.
    // https://epubs.siam.org/doi/abs/10.1137/0213035


    size_t n = hg.getVSize();
    size_t m = hg.getESize();


    // --- Maximum Cardinality Search ---

    size_t eCtr = -1;  // k in paper.

    // States for each vertex if it has been processed before.
    // Corresponds to alpha in the paper.
    vector<bool> vProcessed;
    vProcessed.resize(n, false);

    // Order in which hyperedges are processed.
    // Does not always contain all hyperedges.
    // R in paper.
    int eOrder[m];
    for (size_t i = 0; i < m; i++) eOrder[i] = -1;

    // States for each vertex the index of the hyperedge in which it was first discovered.
    // Corresponds to index of hyperedge that is root of subtree of hyperedges containing that vertex.
    // beta (for vertices) in paper.
    vector<size_t> vRootIdx(n, -1);

    // States for each hyperedge E the root-index of the vertex in E that was processed last.
    // Corresponds with the parent of E in the join tree.
    // gamma in paper.
    vector<int> parIdx(m, -1);

    // Counts how many vertices in each hyperedge are already marked.
    // Allows to skip hyperedges with all vertices marked.
    // Does not affect correctness, only there for speed-up.
    vector<size_t> eSize(m, 0);

    for (MaxCardinalitySet sets(m); !sets.isEmpty(); )
    {
        int S = sets.removeMax();

        // Skip hyperedges with all vertices marked.
        if (eSize[S] == hg[S].size()) continue;

        eCtr++;
        eOrder[eCtr] = S;
        eSize[S] = hg[S].size();

        for (const int& vId : hg[S])
        {
            if (vProcessed[vId]) continue;

            vProcessed[vId] = true;
            vRootIdx[vId] = eCtr;

            for (const int& eId : hg(vId))
            {
                if (eId == S) continue;

                parIdx[eId] = eCtr;

                sets.increaseSize(eId);
                eSize[eId]++;
            }
        }
    }


    // --- Check if acyclic. ---

    // The check is based on Theorem 5 and the code after it in the paper.
    // Informally, the hypergraph is acyclic if and only if, for all hyperedges S,
    // all vertices in S which were discovered before processing S are also in the parent of S.


    // Stores the hyperedges based on their parent-index.
    // That is, childIds[i] stores all hyperedges S with parIdx[S] == i.
    vector<vector<int>> childIds(m);

    for (int eId = 0; eId < m; eId++)
    {
        if (parIdx[eId] < 0) continue;
        childIds[parIdx[eId]].push_back(eId);
    }

    // States for each vertex the last "parent" it was in.
    // index in paper.
    vector<size_t> vLastIdx(n, -1);

    // Iterate over all processed hyperedges ...
    for (int eIdx = 0; eIdx <= eCtr; eIdx++)
    {
        int eId = eOrder[eIdx];

        // Mark all vertices.
        for (const int& vId : hg[eId])
        {
            vLastIdx[vId] = eIdx;
        }

        // ... and check all their children.
        for (const int& S : childIds[eIdx])
        {
            // Check if all vertices in S which were discovered before processing S are also in the parent of S.
            for (const int& vId : hg[S])
            {
                // Let P be the parent of S.
                // The following observation was not shown in the paper.

                // Claim: v was discovered before S and is not in P if and only
                //        if vRootIdx[v] < eIdx and vLastIdx[vId] < eIdx.

                // Proof:
                // =>
                // Let X be the hyperedge in which v was discovered first.
                // If X was processed before P (true for all ancestors of S), then vRootIdx[v] < eIdx.
                // Now assume X was processed after P.
                // Since X shares a vertex with S, line "parIdx[eId] = eCtr" would make X the parent of S.
                // Hence, vRootIdx[v] < eIdx and X was processed before P.
                // Additionally, since v not in P but in X (which was already processed), vLastIdx[vId] < eIdx.
                //
                // <=
                // Clearly, if v was discovered in S or P, then vRootIdx[v] >= eIdx.
                // Additionally, if v in P, then vLastIdx[vId] == eIdx.


                if (vRootIdx[vId] < eIdx && vLastIdx[vId] < eIdx)
                {
                    // Not acyclic.
                    return vector<int>();
                }
            }
        }
    }

    // Hypergraph is acyclic.


    // --- Compute join tree. ---

    vector<int> joinTree;
    joinTree.resize(m, -1);

    for (int eId = 0; eId < m; eId++)
    {
        int pIdx = parIdx[eId];
        if (pIdx < 0) continue;

        int pId = eOrder[pIdx];
        joinTree[eId] = pId;
    }

    return joinTree;
}

// Computes the separator hypergraph for a given acyclic hypergraph with a given join tree.
Hypergraph AlphaAcyclic::separatorHG(const Hypergraph& hg, const vector<int>& joinTree)
{
    // Implementation of getJoinTree() always makes highest-ID hyperedge the root.
    // That is convinient, because we can make the separator HG have matching IDs
    // while still having one fewer hyperedge.

    size_t m = hg.getESize();
    vector<intPair> hgPairs;

    // Process all hyperedges.
    for (int eId = 0; eId < m; eId++)
    {
        int pId = joinTree[eId];
        if (pId < 0) continue; // Skip root.

        const vector<int>& eList = hg[eId];
        const vector<int>& pList = hg[pId];

        for (size_t e = 0, p = 0; e < eList.size() && p < pList.size(); )
        {
            int evId = eList[e];
            int pvId = pList[p];

            if (evId <= pvId) e++;
            if (evId >= pvId) p++;

            if (evId == pvId) hgPairs.push_back(intPair(eId, evId));
        }
    }

    return Hypergraph(hgPairs);
}


// Runs a DFS on the given join tree and returns a pre- and post-order.
// The returned orders state for a given vertex its index in that order.
AlphaAcyclic::orderPair AlphaAcyclic::joinTreeDfs(const vector<int>& joinTree, int rootId)
{
    const int n = joinTree.size();


    // --- Determine children of each node. ---

    vector<vector<int>> childIds;
    childIds.resize(n);

    for (int eId = 0; eId < rootId /* skip root */; eId++)
    {
        int pId = joinTree[eId];
        childIds[pId].push_back(eId);
    }


    // --- Prepare DFS. ---

    // Initialise orders.
    orderPair orders;

    vector<size_t>& preOrder = orders.first;
    vector<size_t>& postOrder = orders.second;

    preOrder.resize(n, -1);
    postOrder.resize(n, -1);

    size_t preIdx = 0;
    size_t postIdx = 0;


    // Helpers to compute DFS.
    vector<size_t> childIndex;
    childIndex.resize(n, 0);

    vector<int> stack;
    stack.push_back(rootId);


    // --- Run DFS. ---

    while (stack.size() > 0)
    {
        int eId = stack.back();
        size_t cIdx = childIndex[eId];

        if (cIdx == 0)
        {
            // *** Pre-order for vId ***
            preOrder[eId] = preIdx;
            preIdx++;
        }

        if (cIdx < childIds[eId].size())
        {
            int childId = childIds[eId][cIdx];

            // No need to check if child was visited before,
            // since we only have edges to children.

            stack.push_back(childId);

            childIndex[eId]++;
        }
        else
        {
            // All neighbours checked, backtrack.
            stack.pop_back();

            // *** Post-order for vId ***
            postOrder[eId] = postIdx;
            postIdx++;
        }
    }

    return orders;
}

// Computes the union join graph for a given acyclic hypergraph.
Graph AlphaAcyclic::unionJoinGraph(const Hypergraph& hg, SubsetGraph::ssgAlgo algo)
{
    // Implements Algorithm 2 from my paper.

    // --- Outline ---

    // 1  Find a join tree for H and call Algorithm 1.
    //    Let T be the resulting join tree and S_H the resulting family of
    //    separators (i.e., the hyperedges of S(H)).

    // 2  Use algorithm A to compute the subset graph G_S of S_H.

    // 3  Create a new graph G = (E, E_G) with E_G = ∅.

    // 4  For Each S ∈ S_H

    // 5      Use G_S to determine all separators S' with S ⊆ S' (including S).

    // 6      For each such S', let EE' be the edge of T which S' represents
    //        and let E be the hyperedge farther away from S in T.
    //        Add E to a set bbE of hyperedges.
    //        If S and S' represent the same edge of T, also add E'.

    // 7      Partition bbE into two sets bbE_1 and bbE_2 based on which side
    //        of S they are in T.

    // 8      For each pair E_1, E_2 with E_1 ∈ bbE_1 and E_2 ∈ bbE_2,
    //        add E_1E_2 into E_G.



    // --- Line 1: Compute join tree and separator hypergraph. ---

    vector<int> joinTree = getJoinTree(hg);

    // Implementation of join tree algorithm has the result that the hyperedge
    // with the largest ID is automatically the root.
    int rootId = hg.getESize() - 1;

    Hypergraph sepHg = separatorHG(hg, joinTree);


    // --- Preprocessing for lines 6 and 7. ---

    // To determine which hyperedges are farther away and on which side of a
    // separator they are, we compute pre- and post-order of the join tree.

    orderPair jtDfs = joinTreeDfs(joinTree, rootId);

    vector<size_t>& pre = jtDfs.first;
    vector<size_t>& post = jtDfs.second;


    // --- Line 2: Compute subset graph. ---

    // List of edges.
    vector<intPair> ssgEdges = algo(sepHg);


    // -- Preprocessing for line 5. ---

    // We later want to get the larger sets of a given set.
    // We therefore build a structure that allows to do that easily.

    vector<vector<int>> superSets;
    superSets.resize(sepHg.getESize());

    // Add separators themself.
    for (int sId = 0; sId < superSets.size(); sId++)
    {
        superSets[sId].push_back(sId);
    }

    // Add all subset relations.
    for (const intPair& edge : ssgEdges)
    {
        // An edge points from the large (.first) to the small (.second) set.
        const int& lrgId = edge.first;
        const int& smlId = edge.second;

        superSets[smlId].push_back(lrgId);
    }


    // --- Line 3: Create empty union join graph. ---

    vector<intPair> eList;
    vector<int> wList; // Will all be 0.


    // --- Line 4: Loop over all separators S. ---

    for (int sId = 0; sId < sepHg.getESize(); sId++)
    {
        // --- Line 5: Determine all S' with S ⊆ S' (including S). ---

        const vector<int>& spList = superSets[sId];


        // --- Line 6: For each, S' determine hyperedges it represents. ---
        //     Line 7: Partition then based on their side of S in T.


        // All hyperedges that are below S, i.e., descendants of S in T.
        vector<int> downList; // bbE_1 in paper.

        // All hyperedges that are above S, i.e., not descendants of S in T.
        vector<int> aboveList; // bbE_2 in paper.


        for (const int& spId : spList)
        {
            // Hyperedge below S'.
            // The way the sparator hypergraph is created, each separator has
            // the same ID as the hyperedge below it in the rooted join tree.
            int chiId = spId;

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

            bool sIsDec = pre[sId] > pre[spId] && post[sId] < post[spId];
            bool sIsAnc = pre[spId] > pre[sId] && post[spId] < post[sId];


            // -- Add hyperedges. --

            // Case 1.
            if (spId == sId)
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

// Computes the edges of the subset graph of the separator hypergraph.
// Combines the computation of the separators with Pritchard's algorithm to avoid some overhead.
vector<intPair> separatorSSG(const Hypergraph& hg, const vector<size_t>& postIdx)
{
    // This algorithm combines the computation of a separator hypergraph with
    // Pritchard's algorithm for subset graphs. That way, we save the overhead
    // of computing a hypergraph.

    // Pritchard's algorithm determines for each vertex v the set X_v of
    // hyperedges which contain v. It then intersects these sets.
    // Observe that for each E_i in X_v, v is the the up-separator S_i of E_i if
    // and only if E_i is not the root of the subtree induced by X_v.
    // We use that observation when creating and processing the reduced sets
    // during Pritchard's algorithm.


    const size_t m = hg.getESize();
    const size_t n = hg.getVSize();


    // --- Determine Post-Order. ---

    vector<int> postOrder;
    postOrder.resize(m);

    for (int eId = 0; eId < m; eId++)
    {
        size_t idx = postIdx[eId];
        postOrder[idx] = eId;
    }


    // --- Determine highest hyperedge for each vertex. ---

    vector<size_t> vMaxIdx;
    vMaxIdx.resize(n, m + 1);

    for (size_t eIdx = 0; eIdx < m; eIdx++)
    {
        int eId = postOrder[eIdx];
        const vector<int>& vLst = hg[eId];

        for (const int& vId : vLst)
        {
            vMaxIdx[vId] = eIdx;
        }
    }


    // --- Create reduced sets for step 3. ---

    ReducedSet vSets[n];
    for (int vId = 0; vId < n; vId++)
    {
        // Ignore highest hyperedge that contains v.
        int rootId = postOrder[vMaxIdx[vId]];
        vSets[vId] = ReducedSet(hg(vId), rootId);
    }


    // --- Step 3) of Pritchard's algorithm. ---

    vector<intPair> result;

    for (int yId = 0; yId < m - 1 /* ignore root */; yId++)
    {
        const vector<int>& vertices = hg[yId];
        if (vertices.size() <= 0) throw std::invalid_argument("Invalid hypergraph.");

        // Compute F.y using the following relation:
        // F.y = \bigcup_{d \in y} F.{d}


        // -- Skip all vertices for which the current hyperedge is the root. --

        size_t yPostIdx = postIdx[yId];
        size_t firstIdx = 0;

        for ( ; firstIdx < vertices.size(); firstIdx++)
        {
            int vId = vertices[firstIdx];
            size_t vRoot = vMaxIdx[vId];
            if (vRoot != yPostIdx) break;
        }


        // Initialise intersection with hyperedges of "first" vertex.
        ReducedSet intersection(vSets[vertices[0 + firstIdx]]);

        // Intersect with hyperedges of all other vertices.
        for (int vIdx = 1 + firstIdx /* first done above */; vIdx < vertices.size(); vIdx++)
        {
            int vId = vertices[vIdx];

            size_t vRoot = vMaxIdx[vId];
            if (vRoot == yPostIdx) continue;

            intersection &= vSets[vId];
        }

        // Intersection calculated. Add edges to result.
        for (auto it = intersection.begin(); it != intersection.end(); ++it)
        {
            int xId = *it;
            if (xId == yId) continue;

            result.push_back(intPair(xId, yId));
        }
    }

    Sorting::radixSort(result);
    return result;
}

// Computes the union join graph for a given acyclic hypergraph.
Graph AlphaAcyclic::unionJoinGraph(const Hypergraph& hg)
{
    // --- Line 1: Compute join tree and separator hypergraph. ---

    vector<int> joinTree = getJoinTree(hg);

    // Implementation of join tree algorithm has the result that the hyperedge
    // with the largest ID is automatically the root.
    int rootId = hg.getESize() - 1;


    // --- Preprocessing for lines 6 and 7. ---

    // To determine which hyperedges are farther away and on which side of a
    // separator they are, we compute pre- and post-order of the join tree.

    orderPair jtDfs = joinTreeDfs(joinTree, rootId);

    vector<size_t>& pre = jtDfs.first;
    vector<size_t>& post = jtDfs.second;


    // --- Line 2: Compute subset graph. ---

    // List of edges.
    vector<intPair> ssgEdges = separatorSSG(hg, post);


    // -- Preprocessing for line 5. ---

    // We later want to get the larger sets of a given set.
    // We therefore build a structure that allows to do that easily.

    vector<vector<int>> superSets;
    superSets.resize(hg.getESize() - 1);

    // Add separators themself.
    for (int sId = 0; sId < superSets.size(); sId++)
    {
        superSets[sId].push_back(sId);
    }

    // Add all subset relations.
    for (const intPair& edge : ssgEdges)
    {
        // An edge points from the large (.first) to the small (.second) set.
        const int& lrgId = edge.first;
        const int& smlId = edge.second;

        superSets[smlId].push_back(lrgId);
    }


    // --- Line 3: Create empty union join graph. ---

    vector<intPair> eList;
    vector<int> wList; // Will all be 0.


    // --- Line 4: Loop over all separators S. ---

    for (int sId = 0; sId < hg.getESize() - 1; sId++)
    {
        // --- Line 5: Determine all S' with S ⊆ S' (including S). ---

        const vector<int>& spList = superSets[sId];


        // --- Line 6: For each, S' determine hyperedges it represents. ---
        //     Line 7: Partition then based on their side of S in T.


        // All hyperedges that are below S, i.e., descendants of S in T.
        vector<int> downList; // bbE_1 in paper.

        // All hyperedges that are above S, i.e., not descendants of S in T.
        vector<int> aboveList; // bbE_2 in paper.


        for (const int& spId : spList)
        {
            // Hyperedge below S'.
            // The way the sparator hypergraph is created, each separator has
            // the same ID as the hyperedge below it in the rooted join tree.
            int chiId = spId;

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

            bool sIsDec = pre[sId] > pre[spId] && post[sId] < post[spId];
            bool sIsAnc = pre[spId] > pre[sId] && post[spId] < post[sId];


            // -- Add hyperedges. --

            // Case 1.
            if (spId == sId)
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
