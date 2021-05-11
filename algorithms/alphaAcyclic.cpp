#include "alphaAcyclic.h"


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
}
