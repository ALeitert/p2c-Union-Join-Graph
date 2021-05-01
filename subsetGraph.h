// Provides algorithms to compute the subset graph of a given hypergraph.

#ifndef __SubsetGraph_H__
#define __SubsetGraph_H__

#include <vector>

#include "helper.h"
#include "hypergraph.h"


namespace SubsetGraph
{
    // Implements a naive approach to find all subset relations (compairs all pairs of hyperedges).
    vector<intPair> naive(const Hypergraph& hg);
}

#endif
