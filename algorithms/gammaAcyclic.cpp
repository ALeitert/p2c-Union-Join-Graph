#include "alphaAcyclic.h"
#include "gammaAcyclic.h"
#include "sorting.h"


// Genrates a gamma-acyclic hypergraph with n vertices and m hyperedges.
Hypergraph GammaAcyclic::genrate(size_t m, size_t n)
{
    // Adjacency lists.
    vector<vector<int>> vList(n);
    vector<vector<int>> eList(m);

    // Available vertices and hyperedges.
    vector<int> allV(n);
    vector<int> allE(m);

    size_t vCount = 0;
    size_t eCount = 0;
    size_t N = 0;

    Sorting::makePermutation(allV.data(), n);
    Sorting::makePermutation(allE.data(), m);


    // --- Create first vertex-hyperedge pair. ---

    // First pair.
    int v0 = allV[vCount];
    int e0 = allE[eCount];

    // "Remove" from list.
    vCount++;
    eCount++;
    N++;

    // Add to hypergraph.
    vList[v0].push_back(e0);
    eList[e0].push_back(v0);


    // --- Generate remaining hypergraph. ---

    for (size_t total = n + m - (vCount + eCount); total > 0; total--)
    {
        size_t rnd = rand() % total;
        size_t par = rand() % (vCount + eCount);

        bool newIsV = rnd < n - vCount;
        bool parIsV = par < vCount;

        if (newIsV && parIsV)
        {
            // New vertex is false twin (of existing vertex).

            int vId = allV[vCount];
            int pId = allV[par];

            // Copy set of hyperedges.
            vList[vId] = vList[pId];

            // Add vertex to hyperedges.
            for (const int& eId : vList[vId])
            {
                eList[eId].push_back(vId);
            }

            vCount++;
            N += vList[vId].size();
        }

        if (newIsV && !parIsV)
        {
            // New vertex is leaf (adjacent to hyperedge).

            int vId = allV[vCount];
            int eId = allE[par - vCount];

            // Make vertex adjecent to hyperedge.
            vList[vId].push_back(eId);
            eList[eId].push_back(vId);

            vCount++;
            N++;
        }

        if (!newIsV && parIsV)
        {
            // New hyperedge is leaf (adjacent to vertex).

            int eId = allE[eCount];
            int vId = allV[par];

            // Make vertex adjecent to hyperedge.
            vList[vId].push_back(eId);
            eList[eId].push_back(vId);

            eCount++;
            N++;
        }

        if (!newIsV && !parIsV)
        {
            // New hyperedge is false twin (of existing hyperedge).

            int eId = allE[eCount];
            int pId = allE[par - vCount];

            // Copy set of vertices.
            eList[eId] = eList[pId];

            // Add hyperedge to vertices.
            for (const int& vId : eList[eId])
            {
                vList[vId].push_back(eId);
            }

            eCount++;
            N += eList[eId].size();
        }
    }

    vector<intPair> hg;
    hg.reserve(N);

    for (int eId = 0; eId < m; eId++)
    {
        const vector<int>& vSet = eList[eId];

        for (const int& vId : vSet)
        {
            hg.push_back(intPair(eId, vId));
        }
    }

    return Hypergraph(hg);
}

// Computes the union join graph for a given gamma-acyclic hypergraph.
Graph GammaAcyclic::unionJoinGraph(const Hypergraph& hg)
{
    // Lemma: An acyclic hypergraph is γ-acyclic if and only if its line graph
    //        is isomorphic to its union join graph.

    // Not that the dual hypergraph H* of a gamma-acyclic hypergraph H is gamma-
    // We can, therefore, compute L(H) = 2Sec(H*) in linear time by using the
    // join tree of H*.


    const Hypergraph& dual = hg.getDual();

    const size_t n = dual.getVSize();
    const size_t m = dual.getESize();


    // --- Compute join tree and DFS on it. ---

    vector<int> dualJoinTree = AlphaAcyclic::getJoinTree(dual);

    // Determine root.
    // The used join tree algorithm normally uses the largest ID as root.
    // Hence we chack from the largest to thesmallest ID.
    int rootId = -1;
    for (rootId = m - 1; rootId >= 0 && dualJoinTree[rootId] >= 0; rootId--) { }

    // Run DFS.
    vector<size_t> dfs = AlphaAcyclic::joinTreeDfs(dualJoinTree, rootId).first;

    // The computed DFS states, for a given node-ID, the index of that node in
    // the pre-order of the tree.
    // The following allows us to determine an ID based on an index.

    vector<int> preOrder(m);

    for (int eId = 0; eId < m; eId++)
    {
        size_t eIdx = dfs[eId];
        preOrder[eIdx] = eId;
    }


    // --- Compute 2-Section graph. ---

    vector<intPair> edgeList;

    vector<int> oldIds;
    vector<int> newIds;
    vector<bool> flagged(n, false);

    for (size_t i = 0; i < m; i++)
    {
        int eId = preOrder[i];
        const vector<int>& vList = dual[eId];


        // Partition into new (not flagged) and old (flagged) vertices.
        for (const int& vId : vList)
        {
            if (flagged[vId])
            {
                oldIds.push_back(vId);
            }
            else
            {
                newIds.push_back(vId);
                flagged[vId] = true;
            }
        }

        // Connect new vertices with old vertices.
        for (const int& nId : newIds)
        {
            for (const int& oId : oldIds)
            {
                edgeList.push_back(intPair(oId, nId));
            }

            oldIds.push_back(nId);
        }

        oldIds.clear();
        newIds.clear();
    }


    // --- Process edges to create graph. ---

    // Make sure first ID is larger.
    for (intPair& edge : edgeList)
    {
        if (edge.first < edge.second)
        {
            swap(edge.first, edge.second);
        }
    }

    Sorting::radixSort(edgeList);
    return Graph(edgeList, vector<int>(edgeList.size(), 0) /* weights */);
}


// Anonymous namespace with helper functions for prune().
namespace
{
    typedef pair<vector<size_t>, vector<size_t>> listPair;

    // Computes distances in the incidence graph of a given hypergraph.
    listPair bfs(const Hypergraph& h, int sId, bool onVertex)
    {
        const size_t n = h.getVSize();
        const size_t m = h.getESize();


        // --- Initialise search. ---

        vector<int> vQ;
        vector<int> eQ;

        vector<bool> vInQueue(n, false);
        vector<bool> eInQueue(m, false);

        pair<vector<size_t>, vector<size_t>> result;
        vector<size_t>& vDist = result.first;
        vector<size_t>& eDist = result.second;

        vDist.resize(n, -1);
        eDist.resize(m, -1);

        // Set start vertex.
        if (onVertex)
        {
            vQ.push_back(sId);
            vInQueue[sId] = true;
        }
        else
        {
            eQ.push_back(sId);
            eInQueue[sId] = true;
        }


        // --- Run BFS. ---

        for (size_t dist = 0; ; dist++, onVertex = !onVertex)
        {
            vector<int>& currQ = onVertex ? vQ : eQ;
            vector<int>& nextQ = onVertex ? eQ : vQ;

            vector<bool>& inQueue = onVertex ? eInQueue : vInQueue;
            vector<size_t>& dList = onVertex ? vDist : eDist;

            // End search if everything processed.
            if (currQ.size() == 0) break;

            nextQ.clear();

            for (const int& xId : currQ)
            {
                dList[xId] = dist;

                const vector<int>& xNeighs = onVertex ? h(xId) : h[xId];

                for (const int& yId : xNeighs)
                {
                    if (inQueue[yId]) continue;

                    nextQ.push_back(yId);
                    inQueue[yId] = true;
                }
            }
        }

        return result;
    }

    // Determine the number of lower neighbours for each vertex of the incidence graph.
    listPair getInnerDegrees
    (
        const Hypergraph& h,
        const listPair& id2Layer
    )
    {
        const size_t n = h.getVSize();
        const size_t m = h.getESize();

        const vector<size_t>& v2Layer = id2Layer.first;
        const vector<size_t>& e2Layer = id2Layer.second;

        listPair innerDegrees;
        vector<size_t>& vDegrees = innerDegrees.first;
        vector<size_t>& eDegrees = innerDegrees.second;

        vDegrees.resize(n, 0);
        eDegrees.resize(m, 0);

        // Degrees of vertices.
        for (int vId = 0; vId < n; vId++)
        {
            size_t vLayer = v2Layer[vId];
            size_t& vCount = vDegrees[vId];

            for (const int& eId : h(vId))
            {
                // Neighbour is in a lower layer?
                if (e2Layer[eId] < vLayer) vCount++;
            }
        }

        // Degrees of hyperedges.
        for (int eId = 0; eId < m; eId++)
        {
            size_t eLayer = e2Layer[eId];
            size_t& eCount = eDegrees[eId];

            for (const int& vId : h[eId])
            {
                // Neighbour is in a lower layer?
                if (v2Layer[vId] < eLayer) eCount++;
            }
        }

        return innerDegrees;
    }

    // Sorts vertices within layers by inner degree of vertices.
    // Does not change the given layers and returns a new list of layers instead.
    vector<vector<int>> sortByDegree
    (
        const vector<size_t>& id2Layer,
        const vector<size_t>& innerDegree
    )
    {
        const size_t k = id2Layer.size();


        // --- Counting sort. ---

        // Result of sorting.
        vector<int> byDegree(k);

        // Counter for counting sort.
        vector<size_t> counter;


        // Count.
        for (int xId = 0; xId < k; xId++)
        {
            size_t key = innerDegree[xId];
            if (key >= counter.size()) counter.resize(key + 1);
            counter[key]++;
        }

        // Pre-fix sums.
        for (size_t i = 1; i < counter.size(); i++)
        {
            counter[i] += counter[i - 1];
        }

        // Sort.
        for (int xId = k - 1; xId >= 0; xId--)
        {
            size_t key = innerDegree[xId];

            counter[key]--;
            size_t idx = counter[key];

            byDegree[idx] = xId;
        }


        // -- Rebuild layers. --

        vector<vector<int>> layers;

        // Add vertices into layers again.
        for (size_t idx = 0; idx < k; idx++)
        {
            int xId = byDegree[idx];
            size_t xLayer = id2Layer[xId] >> 1;

            if (xLayer >= layers.size()) layers.resize(xLayer + 1);
            layers[xLayer].push_back(xId);
        }

        return layers;
    }

    // Contracts a subgraph induced by the given vertices and adds its pruning
    // sequence to the given list.
    // Return the ID into which the subgraph is contracted.
    int contractSG(const vector<int> xList, vector<DistH::Pruning>& result, int mod)
    {
        // Check trival cases.
        // Needed to avoid errors.
        if (xList.size() == 0) return -1;
        if (xList.size() == 1) return xList[0];


        // Since we process hypergraphs, the subgraph has no edges. Therefore,
        // each pruneing operation is a FalseTwin operation.

        for (size_t i = 1; i < xList.size(); i++)
        {
            const int& xId = xList[i - 1];
            const int& pId = xList[i];
            const DistH::PruningType ft = DistH::PruningType::FalseTwin;

            result.push_back(DistH::Pruning(xId + mod, ft, pId + mod));
        }

        return xList.back();
    }
}

// Computes a pruning sequence for a given gamma-acyclic hypergraph.
// Returns an empty list if the given hypergraph is not gamma-acyclic.
vector<DistH::Pruning> GammaAcyclic::pruningSequence(const Hypergraph& h)
{
    throw runtime_error("Not implemented.");
}
