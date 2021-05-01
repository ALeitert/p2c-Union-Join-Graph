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
}
