#include "interval.h"
#include "sorting.h"


// Genrates an interval hypergraph with m edges and total size N.
Hypergraph Interval::genrate(size_t m, size_t N)
{
    // The algorithm is almost the same as for acyclic hypergraphs.
    // The difference is that the tree for interval hypergraphs is just a path.


    // --- Generate a random tree. ---

    // Random permutation.
    int edgeIds[m];
    Sorting::makePermutation(edgeIds, m - 1);

    // We ensure that largest ID is last because we want it to become the root.
    int rootId = m - 1;
    edgeIds[rootId] = rootId;

    int parIds[m];
    parIds[rootId] = -1;

    for (size_t i = 1; i < m; i++)
    {
        // Parent is succeding hyperedge.
        int eId = edgeIds[i - 1];
        int pId = edgeIds[i];

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

// Computes a join path of a given hypergraph.
// Returns a list that contains the parent-ID for each hyperedge.
// Returns an empty list if the hypergraph is not an interval hypergraph.
vector<int> getJoinPath(const Hypergraph& hg)
{
    throw runtime_error("Not implemented.");

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
    // 11          Pick an unprocessed vertex x in pivots (throw away processed ones) and let C be the set of all maximal cliques containing X.
    // 12          Let X_a and X_b be the first and last classes containing a member of C.
    // 13          Replace X_a by X_a \ C, X_a \cap C and X_b by X_b \cap C, X_b \ C.

    // 14      For Each remaining tree edge (C_i, C_j) connecting a clique C_i in C to a clique C_j not in C
    // 15          Push C_i \cap C_j onto pivots.
    // 16          Remove (C_i, C_j) from the clique tree.

    // 17  For Each vertex x
    // 18      If the cliques containing x are not consecutive in the ordering Then
    // 19          Return "Not interval."
    // 20      Return "Interval."



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

    return vector<int>();
}