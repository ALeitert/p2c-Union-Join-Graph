#include "../algorithms/alphaAcyclic.h"
#include "../algorithms/mstEdges.h"
#include "../algorithms/sorting.h"
#include "unionJoinTest.h"


// Computes the union join graph of a given acyclic hypergraph using my
// algorithm with Pritchard's algorithm for subset graphs.
Graph Testing::UnionJoinTest::ujgLeitertPritchard(const Hypergraph& hg)
{
    return AlphaAcyclic::unionJoinGraph(hg, SubsetGraph::pritchardReduced);
}

// Computes the union join graph of a given acyclic hypergraph using the
// linegraph and a modification of Kruskal's algorithm.
Graph Testing::UnionJoinTest::ujgLineKruskal(const Hypergraph& hg)
{
    Graph lg = hg.getLinegraph();

    vector<intPair> edges = MstEdges::kruskal(lg);
    Sorting::radixSort(edges);

    vector<int> weights;
    weights.resize(edges.size(), 0);

    return Graph(edges, weights);
}
