#include <stdexcept>

#include "reducedSet.h"
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
vector<intPair> SubsetGraph::pritchardSimple(const Hypergraph& hg)
{
    // Pritchard assumes for their algorithm that there are no two equal sets.
    // For now, we do not address that problem. If needed, we simplify the hypergraph later.

    // We assune the following about the given hypergraph:
    //   - The internal adjacency lists are sorted.
    //   - Each hyperedge contains at least one vertex.
    //   - Lists do not contain duplicates.


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


    // --- Step 3) ---

    vector<intPair> result;

    for (int yId = 0; yId < hg.getESize(); yId++)
    {
        const vector<int>& vertices = hg[yId];
        if (vertices.size() <= 0) throw std::invalid_argument("Invalid hypergraph.");

        // Compute F.y using the following relation:
        // F.y = \bigcup_{d \in y} F.{d}


        // Initialise intersection with hyperedges of "first" vertex.
        vector<int> intersection(hg(vertices[0]));

        // Intersect with hyperedges of all other vertices.
        for (int vIdx = 1 /* 0 done above */; vIdx < vertices.size(); vIdx++)
        {
            int vId = vertices[vIdx];
            const vector<int>& vEdges = hg(vId);

            int newSize = 0;
            for (int i = 0, j = 0; i < intersection.size() && j < vEdges.size();)
            {
                int iEdge = intersection[i];
                int jEdge = vEdges[j];

                if (iEdge == jEdge)
                {
                    intersection[newSize] = intersection[i];
                    newSize++;
                }

                if (iEdge <= jEdge) i++;
                if (iEdge >= jEdge) j++;
            }

            intersection.resize(newSize);
        }

        // Intersection calculated. Add edges to result.
        for (int i = 0; i < intersection.size(); i++)
        {
            int xId = intersection[i];
            if (xId == yId) continue;

            result.push_back(intPair(xId, yId));
        }
    }

    sortPairsRadix(result);
    return result;
}

// Implements Pritchard's algorithm with reduced sets.
vector<intPair> SubsetGraph::pritchardReduced(const Hypergraph& hg)
{
    // Pritchard assumes for their algorithm that there are no two equal sets.
    // For now, we do not address that problem. If needed, we simplify the hypergraph later.

    // We assune the following about the given hypergraph:
    //   - The internal adjacency lists are sorted.
    //   - Each hyperedge contains at least one vertex.
    //   - Lists do not contain duplicates.


    // --- Preliminaries ---

    //     F  The given hypergraph.

    //   F.y  The family of hyperedges x of F such that the hyperedge y is a subset of x.
    //        That is, F.y = { x | x \subseteq y }.

    // F.{d}  The set of all hyperedges x of F such that x contains the vertex d.


    // --- Outline ---

    // 1) Order F, i.e., assign each hyperedge in F a unique index.
    // 2) For each vertex d, compute F.{d}.
    // 3) For each hyperedge y, record edge (x, y) for each x in F.y - y.

    // Steps 1) is already done in the given hypergraph.


    // --- Step 2)  Create reduced sets for each vertex. ---

    const int n = hg.getVSize();
    const int m = hg.getESize();

    ReducedSet vSets[n];
    for (int vId = 0; vId < n; vId++)
    {
        vSets[vId] = ReducedSet(hg(vId));
    }


    // --- Step 3) ---

    vector<intPair> result;

    for (int yId = 0; yId < m; yId++)
    {
        const vector<int>& vertices = hg[yId];
        if (vertices.size() <= 0) throw std::invalid_argument("Invalid hypergraph.");

        // Compute F.y using the following relation:
        // F.y = \bigcup_{d \in y} F.{d}


        // Initialise intersection with hyperedges of "first" vertex.
        ReducedSet intersection(vSets[vertices[0]]);

        // Intersect with hyperedges of all other vertices.
        for (int vIdx = 1 /* 0 done above */; vIdx < vertices.size(); vIdx++)
        {
            int vId = vertices[vIdx];
            intersection &= vSets[vId];
        }

        // Intersection calculated. Add edges to result.
        for (auto it = intersection.begin(); it != intersection.end(); ++it)
        {
            int xId = *it;
            if (xId == yId) continue;

            result.push_back(intPair(xId, yId));
        }
    }

    sortPairsRadix(result);
    return result;
}

// Implements Pritchard's algorithm with presorted hyperedges.
vector<intPair> SubsetGraph::pritchardRefinement(const Hypergraph& hg)
{
    throw runtime_error("Not implemented");

    // Pritchard assumes for their algorithm that there are no two equal sets.
    // For now, we do not address that problem. If needed, we simplify the hypergraph later.

    // We assune the following about the given hypergraph:
    //   - The internal adjacency lists are sorted.
    //   - Each hyperedge contains at least one vertex.
    //   - Lists do not contain duplicates.


    // --- Preliminaries ---

    //     F  The given hypergraph.

    //   F.y  The family of hyperedges x of F such that the hyperedge y is a subset of x.
    //        That is, F.y = { x | x \subseteq y }.

    // F.{d}  The set of all hyperedges x of F such that x contains the vertex d.

    //  w(d)  The weight of a vertex d, defined as the number of hyperedges that contain d.



    // --- Outline ---

    // 1) Order F:
    //    1.1 Compute the weight of each domain element.
    //    1.2 Totally order the domain elements by non-increasing weight;
    //        break ties arbitrarily.
    //    1.3 Order the vertices of each hyperedge with respect to their weight.
    //    1.4 Sort the hyperedges lexicographically, using the ordered vertices as the ordered alphabet.


    // 2) For each vertex d, compute F.{d}.
    // 3) For each hyperedge y, record edge (x, y) for each x in F.y - y.


    int n = hg.getVSize();
    int m = hg.getESize();


    // --- Step 1.2: Sort vertices by weight. ---


    int verSorted[n];

    // We use counting sort.
    int count[m];
    for (int i = 0; i < m; i++) count[i] = 0;

    // Count.
    for (int vId = 0; vId < n; vId++)
    {
        int key = hg(vId).size();
        count[key]++;
    }

    // Prefix sums.
    for (int i = 1; i < m; i++)
    {
        count[i] += count[i - 1];
    }

    // Sort.
    for (int vId = n - 1; vId >= 0; vId++)
    {
        int key = hg(vId).size();
        count[key]--;
        int idx = count[key];
        verSorted[idx] = vId;
    }


    // --- Step 1.3: Sort verticies within hyperedges by their weight. ---

    // Two Options
    //   1) Sort with original vertex IDs.
    //   2) Fill hyperedges with index in sorted order instead.
    // For now, we use option 2.


    vector<int> hyperedges[m];

    // Sort into hyperedges.
    for (int idx = 0; idx < n; idx++)
    {
        int vId = verSorted[idx];
        const vector<int>& vHypEdges = hg(vId);

        for (int eId : vHypEdges)
        {
            hyperedges[eId].push_back(idx); // Option 2.
        }
    }
}
