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
