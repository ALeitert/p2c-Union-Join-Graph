// Provides algorithms to compute the subset graph of a given hypergraph.

#ifndef __Algorithms_SubsetGraph_H__
#define __Algorithms_SubsetGraph_H__

#include <vector>

#include "../helper.h"
#include "../dataStructures/hypergraph.h"


namespace SubsetGraph
{
    // A reference to a function that computes the subset graph of a given hypergraph.
    typedef vector<intPair> (&ssgAlgo)(const Hypergraph&);


    // Implements a naive approach to find all subset relations (compairs all pairs of hyperedges).
    vector<intPair> naive(const Hypergraph& hg);


    // Implements Pritchard's "simple" algorithm as published in Algorithmica 1999.
    vector<intPair> pritchardSimple(const Hypergraph& hg);

    // Implements Pritchard's algorithm with reduced sets.
    vector<intPair> pritchardReduced(const Hypergraph& hg);

    // Implements Pritchard's algorithm with presorted hyperedges.
    vector<intPair> pritchardRefinement(const Hypergraph& hg);
}

#endif
