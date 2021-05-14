// Provides a framework to test algorithms for Union Join Graphs.

#ifndef __Testing_UnionJoinTest_H__
#define __Testing_UnionJoinTest_H__


#include "../dataStructures/graph.h"
#include "../dataStructures/hypergraph.h"


namespace Testing
{
    // A reference to a function that computes the union join graph of a given hypergraph.
    typedef Graph (&ujgAlgo)(const Hypergraph&);


    class UnionJoinTest
    {
    public:

        // Default constructor.
        UnionJoinTest() { /* Does nothing */ }


        // --- Static wrapper functions. ---

        // Computes the union join graph of a given acyclic hypergraph using my
        // algorithm with Pritchard's algorithm for subset graphs.
        static Graph ujgLeitertPritchard(const Hypergraph&);

        // Computes the union join graph of a given acyclic hypergraph using the
        // linegraph and a modification of Kruskal's algorithm.
        static Graph ujgLineKruskal(const Hypergraph&);


    private:

    };
}

#endif
