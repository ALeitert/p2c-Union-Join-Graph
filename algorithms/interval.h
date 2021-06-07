// Provides algorithms for interval hypergraphs.

#ifndef __Algorithms_Interval_H__
#define __Algorithms_Interval_H__


#include "../dataStructures/hypergraph.h"


namespace Interval
{
    // Genrates an interval hypergraph with m edges and total size N.
    Hypergraph genrate(size_t m, size_t N);

    // Computes a join path of a given hypergraph.
    // Returns an order of hyperedges that is a valid join path.
    // Returns an empty list if the hypergraph is not an interval hypergraph.
    vector<int> getJoinPath(const Hypergraph& hg);

    // Computes the edges of the subset graph of the given interval hypergraph.
    // A pair (x, y) states that y is subset of x.
    vector<intPair> subsetGraph(const Hypergraph& hg);

    // Computes the union join graph for a given interval hypergraph.
    // A slightly optimised function that avoids some overhead.
    Graph unionJoinGraph(const Hypergraph& hg);
}

#endif
