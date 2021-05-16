#include <algorithm>

#include "../dataStructures/dijkstraHeap.h"
#include "../dataStructures/unionFind.h"
#include "mstEdges.h"


// Helper function for checkAllEdges().
// Computes, for a given vertex u and all neighbours v, the largest minimum edge weight of all paths from u to v.
vector<int> maxMinWeights(const Graph& g, int startId)
{
    // Assume we are given a vertex u.
    // For all neighbours v, we want to compute the largest minimum edge weight of all paths from u to v.
    // That is: max_P min_xy wei(xy)

    // Note that Dijkstra's algorithm solves a similar problem.
    // The distance between two vertices is the smallest sum of edges of all paths.
    // That is: min_P sum_xy wei(xy)

    // We therefore use a very similar algorithm.
    // We start with Dikstra's algorithm and make the following changes:
    //  1) We relax an edge xy as follows if d(y) < min(d(x), wei(uv)) then d(y) := min(d(x), wei(uv))
    //     We use < because max_P, and min() because min_xy.
    //  2) As next vertex to process, we pick the vertex with largest d().
    //  3) Subsequently from 1) and 2), we set d(u) := +infinite.

    // We can show the correctnes of our approach with the same technique one uses to prove Dijkstra's algorithm.
    // Let v be the next vertex to pick (i.e., with largest d()).
    // Claim: d() is optimal for v.
    // Proof: Assume there is a differen path P leading to a better d(v).
    // Let P = s ~> x -> y ~> v where xy is edge on boundry (i.e., we processed x and can pick y).
    // Note that d(y) <= d(v).
    // Hence, there is an edge e in s ~> y with wei(e) <= d(v).
    // Subsequently, min_P <= wei(e) < d(v).


    // Create heap.
    DijkstraHeap heap(g.size());
    const int* const distances = heap.getWeights();

    int startWei = numeric_limits<int>::max();
    heap.update(startId, -startWei); // times -1 since heap is min-heap.

    while (heap.getSize() > 0)
    {
        int uId = heap.removeMin();

        // Unreachable vertex.
        if (distances[uId] == numeric_limits<int>::max()) break;

        int uDist = -distances[uId]; // times -1 since heap is min-heap.

        // Relax all neighbours.
        for (int nIdx = 0; nIdx < g[uId].size(); nIdx++)
        {
            int vId = g[uId][nIdx];
            int vDist = -distances[vId]; // times -1 since heap is min-heap.

            int uvWei = g(uId)[nIdx];
            int uvDist = min(uDist, uvWei);

            if (vDist < uvDist)
            {
                heap.update(vId, -uvDist); // times -1 since heap is min-heap.
            }
        }
    }


    // --- Fill result. ---
    vector<int> result;
    for (size_t i = 0; i < g[startId].size(); i++)
    {
        int vId = g[startId][i];
        int vDist = -distances[vId]; // times -1 since heap is min-heap.
        result.push_back(vDist);
    }

    return result;
}

// Determines all edges which are part of a MaxST by checking each edge individually.
vector<intPair> MstEdges::checkAllEdges(const Graph& g)
{
    // Consider an edge uv with weigth w.

    // --- Theorem ---
    // There is a MaxST T that contains uv if and only if
    // there is no path from u to v where each edge weight is strictly larger than w
    // (== all path contain an edge with weight <= w).

    // Proof (outline):
    // -->  Assume uv is part of some MaxST T and there is such a path P.
    //      Each edge P then has a larger weight, and one such edge xy is not part of T.
    //      Hence, T'created by removing uv and adding xy has a larger total weight than T.
    //
    // <--  Assume there is no such path and T is a MaxST not containing uv.
    //      Let P be the path from u to v in T.
    //      P contains and edge xy with weight <= w.
    //      Let T' be tree generated by removing xy and adding uv.
    //      Then wei(T') >= wei(T), since wei(uv) >= wei(xy).


    // --- Approach ---

    // 'There is no path from u to v where each edge weight is strictly larger than w.'
    // == not [  exist P = u ~> v:  forall xy in P:   wei(xy) >  w ]
    // ==       forall P = u ~> v:   exist xy in P:   wei(xy) <= w
    // ==       forall P = u ~> v:   min_{ xy in P }  wei(xy) <= w
    // ==        max_{ P = u ~> v }  min_{ xy in P }  wei(xy) <= w
    // == 'The largest minimum edge weight of all paths is at most w.'

    // We simplify the statement as follows: max_P min_xy wei(xy) <= w.

    // Our goal now is to compute the largest minimum edge weight of all paths and all pairs uv.
    // The function maxMinWeights(u) computes that for a given u and all vertices v.

    // We now compute all edges that are part of a MaxST as follows:
    // For each vertex u, let A[] = maxMinWeights(u).
    // For each neighbour v of u, if A[v] <= wei(uv), add uv to result.


    vector<intPair> result;

    for (size_t uId = 0; uId < g.size(); uId++)
    {
        // Compute largest minimum edge weight of all paths to neighbours.
        vector<int> mmW = maxMinWeights(g, uId);

        // Compare to edge weight of each neighbour.
        for (size_t j = 0; j < g[uId].size(); j++)
        {
            size_t vId = g[uId][j];
            int uvWei = g(uId)[j];

            // Check if max_P min_xy wei(xy) <= w.
            // If so, add to output if uId < vId.
            if (mmW[j] <= uvWei && uId < vId)
            {
                result.push_back(sizePair(uId, vId));
            }
        }
    }

    return result;
}

// Determines all edges which are part of a MaxST based on Kruskal's algorithm.
vector<intPair> MstEdges::kruskal(const Graph& g)
{
    // Kruskal's algorithm computes a MST by first sorting all edges by weight.
    // Edges are then processed in that order.
    // If there are multiple edges to add which exclude each other, ties are broken by the sorting:
    // an edge that is earlier in the sorting will be picked.

    // One can modify the algorithm to find all edges of a MST as follows.
    // For the current edge, collect all edges uv with the current weight.
    // Before adding any edge to the answer, check for each edge if find(u) != find(v).
    // If that is the case, add uv to the answer. Afterwards, call union(u, v) on all these edges.


    // --- Create a list of all edges and sort them. ---

    vector<pair<int, intPair>> edgeList;

    for (int uId = 0; uId < g.size(); uId++)
    {
        const vector<int>& neighs = g[uId];
        const vector<int>& weights = g(uId);

        for (size_t vIdx = 0; vIdx < neighs.size(); vIdx++)
        {
            int vId = neighs[vIdx];
            if (vId >= uId) break;

            int uvWei = weights[vIdx];
            edgeList.push_back(pair<int, intPair>(uvWei, intPair(uId, vId)));
        }
    }


    // --- Sort edges (using counting sort). ---

    {
        vector<pair<int, intPair>> buffer;
        buffer.resize(edgeList.size());

        const size_t n = g.size();

        vector<size_t> count;
        count.resize(n, 0);

        // Count keys.
        for (size_t i = 0; i < edgeList.size(); i++)
        {
            int key = edgeList[i].first;
            count[key]++;
        }

        // Postfix sum (we want decreasing order).
        for (size_t i = count.size() - 2; i < count.size(); i--)
        {
            count[i] += count[i + 1];
        }

        // Sort.
        for (size_t i = edgeList.size() - 1; i < edgeList.size(); i--)
        {
            int key = edgeList[i].first;

            count[key]--;
            size_t idx = count[key];

            buffer[idx] = edgeList[i];
        }

        buffer.swap(edgeList);
    }


    // --- Process edges. ---

    vector<intPair> result;
    vector<intPair> buffer;

    UnionFind uf(g.size());

    for (size_t ePtr = 0; ePtr < edgeList.size();)
    {
        int curWei = edgeList[ePtr].first;

        // Add all edges with same weight that allow to join two sets.
        for (; ePtr < edgeList.size() && curWei == edgeList[ePtr].first; ePtr++)
        {
            int uId = edgeList[ePtr].second.first;
            int vId = edgeList[ePtr].second.second;

            if (uf.findSet(uId) != uf.findSet(vId))
            {
                buffer.push_back(intPair(uId, vId));
            }
        }

        // Process buffered edges.
        for (size_t i = 0; i < buffer.size(); i++)
        {
            intPair& edge = buffer[i];

            int uId = edge.first;
            int vId = edge.second;

            uf.unionSets(uId, vId);
            result.push_back(edge);
        }

        buffer.clear();
    }

    return result;
}
