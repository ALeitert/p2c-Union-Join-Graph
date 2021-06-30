// Contains algorithms for gamma-acyclic hypergraphs.

#ifndef __Algorithms_GammaAcyclic_H__
#define __Algorithms_GammaAcyclic_H__


#include "../dataStructures/hypergraph.h"
#include "distHered.h"


namespace GammaAcyclic
{
    // Genrates a gamma-acyclic hypergraph with n vertices and m hyperedges.
    Hypergraph genrate(size_t m, size_t n);

    // Computes the union join graph for a given gamma-acyclic hypergraph.
    Graph unionJoinGraph(const Hypergraph& hg);


    // Computes a pruning sequence for a given gamma-acyclic hypergraph.
    // Returns an empty list if the given hypergraph is not gamma-acyclic.
    vector<DistH::Pruning> pruningSequence(const Hypergraph& h);

    // Computes the edges of the subset graph of the given gamma-acyclic hypergraph.
    // A pair (x, y) states that y is subset of x.
    vector<intPair> subsetGraph(const Hypergraph& h);
}

#endif
