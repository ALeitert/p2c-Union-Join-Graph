// Generates hypergraph to test algorithms that compute subset graphs.

#ifndef __Testing_SubsetTest_H__
#define __Testing_SubsetTest_H__


#include "../algorithms/subsetGraph.h"
#include "../hypergraph.h"

namespace Testing
{
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
}

#endif
