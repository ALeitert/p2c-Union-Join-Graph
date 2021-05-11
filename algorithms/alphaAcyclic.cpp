#include "alphaAcyclic.h"
#include "sorting.h"


// Genrates an alpha-acyclic hypergraph with m edges and total size N.
Hypergraph AlphaAcyclic::genrate(size_t m, size_t N)
{
    throw runtime_error("Not implemented.");


    // --- Outline ---

    // - Generate tree of size m.
    // - Determine size of each hyperedge.
    //   * Each gets at least ove vertex.
    //   * Then, sizes are increased randomly until total size is N.
    // - Determine shared vertices between adjacent hyperedges.
    //   * Determine number s >= 1 randomly.
    //   * Pick s random vertices from parent.
    //   * Add these to child and then fill it with new vertices.
    // - Shuffle IDs and create hypergraph.


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
}
