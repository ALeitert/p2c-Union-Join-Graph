// Contains functions to test various implementations.

#ifndef __Testing_Testing_H__
#define __Testing_Testing_H__


#include "../algorithms/subsetGraph.h"
#include "unionJoinTest.h"


namespace Testing
{
    // --- Subset Graph ---

    // Tests a given function that computes the subset graph of a given hypergraph.
    void testGeneralSSG(SubsetGraph::ssgAlgo ssg, int seed, int tests, int maxSize);


    // --- Union Join Graph ---

    // Tests a given function that computes the subset graph of a given hypergraph.
    // Returns the time needed in milliseconds.
    uint64_t unionJoinGraph(ujgAlgo algo, string name, unsigned int seed, size_t tests, size_t maxSize);
}

#endif
