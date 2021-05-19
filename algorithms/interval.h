// Provides algorithms for interval hypergraphs.

#ifndef __Algorithms_Interval_H__
#define __Algorithms_Interval_H__


#include "../dataStructures/hypergraph.h"


namespace Interval
{
    // Genrates an interval hypergraph with m edges and total size N.
    Hypergraph genrate(size_t m, size_t N);

    // Computes a join path of a given hypergraph.
    // Returns a list that contains the parent-ID for each hyperedge.
    // Returns an empty list if the hypergraph is not an interval hypergraph.
    vector<int> getJoinPath(const Hypergraph& hg);
}

#endif