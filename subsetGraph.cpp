#include "subsetGraph.h"


// Implements a naive approach to find all subset relations (compairs all pairs of hyperedges).
vector<intPair> SubsetGraph::naive(const Hypergraph& hg)
{
    int m = hg.getESize();
    vector<intPair> answer;

    for (int i = 0; i < m; i++)
    {
        const vector<int>& iSet = hg[i];

        for (int j = i + 1; j < m; j++)
        {
            const vector<int>& jSet = hg[j];

            bool iSubsetJ = true;
            bool jSubsetI = true;

            for (int pI = 0, pJ = 0; iSubsetJ || jSubsetI;)
            {
                if (pI >= iSet.size() && pJ >= jSet.size()) break;

                if (pI >= iSet.size())
                {
                    // S_j contains an element not in S_i.
                    jSubsetI = false;
                    break;
                }

                if (pJ >= jSet.size())
                {
                    // S_i contains an element not in S_j.
                    iSubsetJ = false;
                    break;
                }

                int vI = iSet[pI];
                int vJ = jSet[pJ];

                iSubsetJ = iSubsetJ && (vI >= vJ);
                jSubsetI = jSubsetI && (vI <= vJ);

                if (vI <= vJ) pI++;
                if (vI >= vJ) pJ++;
            }

            if (iSubsetJ) answer.push_back(intPair(j, i));
            if (jSubsetI) answer.push_back(intPair(i, j));
        }
    }

    sortPairsRadix(answer);
    return answer;
}


// Implements Pritchard's "simple" algorithm as published in Algorithmica 1999.
vector<intPair> pritchardSimple(const Hypergraph& hg)
{
    throw "Not implemented.";

    // Pritchard assumes for their algorithm that there are no two equal sets.
    // For now, we do not address that problem. If needed, we simplify the hypergraph later.

    // We assune that the internal adjacency lists of the given hypergraph are sorted.


    // --- Preliminaries ---

    //     F  The given hypergraph.

    //   F.y  The family of hyperedges x of F such that the hyperedge y is a subset of x.
    //        That is, F.y = { x | x \subseteq y }.

    // F.{d}  The set of all hyperedges x of F such that x contains the vertex d.


    // --- Outline ---

    // 1) Order F, i.e., assign each hyperedge in F a unique index.
    // 2) For each vertex d, compute F.{d}.
    // 3) For each hyperedge y, record edge (x, y) for each x in F.y - y.

    // Steps 1) and 2) are already done in the given hypergraph.
}
