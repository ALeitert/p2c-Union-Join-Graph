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

    return vector<int>();
}
