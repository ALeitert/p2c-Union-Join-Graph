// Contains functions to test various implementations.

#ifndef __Testing_Testing_H__
#define __Testing_Testing_H__


#include "../algorithms/subsetGraph.h"


namespace Testing
{
    // Tests a given function that computes the subset graph of a given hypergraph.
    void testGeneralSSG(SubsetGraph::ssgAlgo ssg, int seed, int tests, int maxSize);
}

#endif
