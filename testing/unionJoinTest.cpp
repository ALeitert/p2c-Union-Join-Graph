#include "../algorithms/alphaAcyclic.h"
#include "unionJoinTest.h"


// Computes the union join graph of a given acyclic hypergraph using my
// algorithm with Pritchard's algorithm for subset graphs.
Graph Testing::UnionJoinTest::ujgLeitertPritchard(const Hypergraph& hg)
{
    return AlphaAcyclic::unionJoinGraph(hg, SubsetGraph::pritchardReduced);
}
