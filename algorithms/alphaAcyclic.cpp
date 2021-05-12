#include "../dataStructures/maxCardinalitySet.h"
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
    throw runtime_error("Not implemented.");

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
    size_t vRootIdx[n];
    for (int vId = 0; vId < n; vId++) vRootIdx[vId] = -1;

    // States for each hyperedge E the root-index of the vertex in E that was processed last.
    // Corresponds with the parent of E in the join tree.
    // gamma in paper.
    int parIdx[m];
    for (int eId = 0; eId < m; eId++) parIdx[eId] = -1;

    // Counts how many vertices in each hyperedge are already marked.
    // Allows to skip hyperedges with all vertices marked.
    // Does not affect correctness, only there for speed-up.
    size_t eSize[m];

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
    vector<int> childIds[m];

    for (int eId = 0; eId < m; eId++)
    {
        if (parIdx[eId] < 0) continue;
        childIds[parIdx[eId]].push_back(eId);
    }

    // States for each vertex the last "parent" it was in.
    // index in paper.
    size_t vLastIdx[n];
    for (int i = 0; i < n; i++)
    {
        vLastIdx[i] = -1;
    }

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

    return vector<int>();
}
