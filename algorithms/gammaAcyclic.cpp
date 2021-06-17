#include "alphaAcyclic.h"
#include "gammaAcyclic.h"
#include "sorting.h"


#include <iostream>

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
