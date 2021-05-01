// Generates hypergraph to test algorithms that compute subset graphs.

#ifndef __SubsetTest_H__
#define __SubsetTest_H__

#include "hypergraph.h"

// A reference to a function that computes the subset graph of a given hypergraph.
typedef vector<intPair> (&ssgFun)(const Hypergraph&);


class SubsetTest
{
public:

    // Default constructor.
    SubsetTest() { /* Does nothing */ }

    // Generates a random hypergraph of the given size and its subsetgraph.
    const Hypergraph& build(int size);

    // Verifies that the given edges represent the subset graph.
    bool verify(const vector<intPair>& edges) const;

    const vector<intPair>& getSolution();

private:

    // The hypergraph to test.
    Hypergraph h;

    // The edges of the subset graph.
    vector<intPair> solution;
};

// Tests a given function that computes the subset graph of a given hypergraph.
void testGeneralSSG(ssgFun ssg, int seed, int tests, int maxSize);

#endif
