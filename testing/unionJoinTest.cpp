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

// Computes the union join graph of a given acyclic hypergraph using my
// algorithm with Pritchard's algorithm for subset graphs.
Graph Testing::UnionJoinTest::ujgLeitertPritchardSpeedUp(const Hypergraph& hg)
{
    return AlphaAcyclic::unionJoinGraph(hg);
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

// Runs two union join graph algorithms and verifies that their output is the same.
bool Testing::UnionJoinTest::compareAlgorithms(const Hypergraph& hg, ujgAlgo algo1, ujgAlgo algo2)
{
    // Compute graphs.
    Graph ujg1 = algo1(hg);
    Graph ujg2 = algo2(hg);

    // Same size?
    if (ujg1.size() != ujg2.size()) return false;

    // Check neighbours of each vertex.
    for (int vId = 0; vId < ujg1.size(); vId++)
    {
        const vector<int>& neiV1 = ujg1[vId];
        const vector<int>& neiV2 = ujg2[vId];

        if (ujg1.size() != ujg2.size()) return false;

        for (int i = 0; i < neiV1.size(); i++)
        {
            if (neiV1[i] != neiV2[i]) return false;
        }
    }

    return true;
}
