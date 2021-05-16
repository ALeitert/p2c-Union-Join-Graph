// Contains algorithms for alpha-acyclic hypergraphs.

#ifndef __Algorithms_AlphaAcyclic_H__
#define __Algorithms_AlphaAcyclic_H__


#include "../dataStructures/graph.h"
#include "../dataStructures/hypergraph.h"
#include "subsetGraph.h"


namespace AlphaAcyclic
{
    // Genrates an alpha-acyclic hypergraph with m edges and total size N.
    Hypergraph genrate(size_t m, size_t N);

    // Computes a join tree of a given hypergraph.
    // Returns a list that contains the parent-ID for each hyperedge.
    // Returns an empty list if the given hypergraph is not acyclic.
    vector<int> getJoinTree(const Hypergraph& hg);

    // Computes the separator hypergraph for a given acyclic hypergraph with a given join tree.
    Hypergraph separatorHG(const Hypergraph& hg, const vector<int>& joinTree);

    // Computes the union join graph for a given acyclic hypergraph.
    Graph unionJoinGraph(const Hypergraph& hg, SubsetGraph::ssgAlgo A);

    // Computes the union join graph for a given acyclic hypergraph.
    Graph unionJoinGraph(const Hypergraph& hg);
}

#endif
