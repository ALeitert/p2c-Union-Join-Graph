#include <stdexcept>

#include "../dataStructures/reducedSet.h"
#include "../helper.h"
#include "sorting.h"
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

    Sorting::radixSort(answer);
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

    Sorting::radixSort(result);
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

    Sorting::radixSort(result);
    return result;
}

// Implements Pritchard's algorithm with presorted hyperedges.
vector<intPair> SubsetGraph::pritchardRefinement(const Hypergraph& hg)
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


    int vWeiOrder[n];

    // We use counting sort.
    int count[m];
    for (int i = 0; i < m; i++) count[i] = 0;

    // Count.
    for (int vId = 0; vId < n; vId++)
    {
        int key = m - hg(vId).size();
        count[key]++;
    }

    // Prefix sums.
    for (int i = 1; i < m; i++)
    {
        count[i] += count[i - 1];
    }

    // Sort.
    for (int vId = n - 1; vId >= 0; vId--)
    {
        int key = m - hg(vId).size();
        count[key]--;
        int idx = count[key];
        vWeiOrder[idx] = vId;
    }


    // --- Step 1.3: Sort verticies within hyperedges by their weight. ---

    // Two Options
    //   1) Sort with original vertex IDs.
    //   2) Fill hyperedges with index in sorted order instead.
    // For now, we use option 2.


    vector<vector<int>> hgHypEdges;
    hgHypEdges.resize(m);

    // Sort into hyperedges.
    for (int voIdx = 0; voIdx < n; voIdx++)
    {
        int vId = vWeiOrder[voIdx];
        const vector<int>& vHypEdges = hg(vId);

        for (int eId : vHypEdges)
        {
            hgHypEdges[eId].push_back(voIdx); // Option 2.
        }
    }


    // --- Step 1.4: Sort the hyperedges lexicographically. ---

    size_t* eLexOrder = Sorting::lexSort(hgHypEdges);


    // --- Step 2)  For each vertex d, compute F.{d}. ---

    // We first recreate, for each vertex v, the set of hyperedges that contain v.
    // We ensure that these are sorted according to eLexOrder[].


    // -- Step 2.1: Sort hyperedges within vertex sets according to lex. order.

    vector<vector<int>> hgVertices;
    hgVertices.resize(n);

    // Sort hyperedge indices into vertices.
    for (int eoIdx = 0; eoIdx < n; eoIdx++)
    {
        int eId = eLexOrder[eoIdx];
        const vector<int>& eVerts = hg[eId];

        for (int vId : eVerts)
        {
            hgVertices[vId].push_back(eoIdx);
        }
    }


    // The vectors 'hgHypEdges' and 'hgVertices' now represent the sorted hypergraph.
    // They are still ordered by the original ID, but they store the indices in the respective orders.


    // -- Step 2.2: Create reduced sets for all vertices. --

    // Reduced sets store indices of hyperedges in the lex. order.

    ReducedSet vSets[n];
    for (int woIdx = 0; woIdx < n; woIdx++)
    {
        int vId = vWeiOrder[woIdx];
        vSets[woIdx] = ReducedSet(hgVertices[vId]);
    }


    // --- Step 3) ---

    vector<intPair> result;
    vector<ReducedSet> history;
    history.push_back(ReducedSet());

    for (int eoIdx = 0, sucShared = 0; eoIdx < n; eoIdx++)
    {
        int yId = eLexOrder[eoIdx];

        const vector<int>& yVertIdxs = hgHypEdges[yId];
        if (yVertIdxs.size() <= 0) throw std::invalid_argument("Invalid hypergraph.");

        size_t ySize = yVertIdxs.size();


        // Compute F.y using the following relation:
        // F.y = \bigcup_{d \in y} F.{d}


        // --- Shared history with previous set? ---

        // Let y' = { v_a, ..., v_p } the previous hyperedge (sorted by weight).
        // Let  y = { v_b, ..., v_q } the current hyperedge (sorted by weight).

        // Note that, since edges are sorted lex., a <= b.
        // We know want to determine the largest index i such that
        // y'[j] == y[j] for all j in [0, i].
        // In that case, we can use the already stored intersection and do not have to recompute it again.

        // Instead of counting how many we share with the preceding set, we do that for the succeding.
        // That way, we make the history only as large as needed.

        int shared = sucShared;

        if (eoIdx + 1 < n)
        {
            sucShared = 0;
            int postId = eLexOrder[eoIdx + 1];
            const vector<int>& postVertIdxs = hgHypEdges[postId];
            size_t minSize = min(ySize, postVertIdxs.size());

            while
            (
                sucShared < minSize &&
                yVertIdxs[sucShared] == postVertIdxs[sucShared]
            )
            {
                sucShared++;
            }
        }


        // --- Initialise intersection with hyperedges of "first" vertex. ---

        ReducedSet intersection;

        if (shared > 0)
        {
            intersection = history[shared - 1];
        }
        else
        {
            int v0_woIdx = yVertIdxs[0]; // Index after sorting by weight.
            intersection = ReducedSet(vSets[v0_woIdx]);
            history[0] = intersection;
        }


        // Intersect with hyperedges of all other vertices.
        for (int i = max(shared, 1) /* previous done above */; i < ySize; i++)
        {
            int vIdx = yVertIdxs[i];
            intersection &= vSets[vIdx];

            if (i < sucShared)
            {
                if (i >= history.size()) history.push_back(intersection);
                else history[i] = intersection;
            }
        }

        // Intersection calculated. Add edges to result.
        for (auto it = intersection.begin(); it != intersection.end(); ++it)
        {
            int x_eoIdx = *it;
            int xId = eLexOrder[x_eoIdx];
            if (xId == yId) continue;

            result.push_back(intPair(xId, yId));
        }
    }


    delete[] eLexOrder;

    Sorting::radixSort(result);
    return result;
}
