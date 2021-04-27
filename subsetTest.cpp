#include <unordered_set>
#include <vector>

#include "helper.h"
#include "subsetTest.h"


// Generates a random DAG of the given size.
// Returns an edge list of the generated graph in topological order.
// Each vertex also contains an edge to itself (allows to identify isolated vertices).
vector<intPair> randomDAG(int size, int avgDeg)
{
    // --- Create edges. ---

    unordered_set<intPair, intPairHash> pSet;
    vector<intPair> list;

    int edges = (avgDeg * size) / 2;

    for (int i = 0; i < edges; i++)
    {
        // fIdx and tIdx are indices in topological order.
        // IDs are assigned later.

        int fIdx = rand() % size;

        // Ensures frId != tIdx
        int tIdx = rand() % (size - 1);
        if (tIdx >= fIdx) tIdx++;

        if (fIdx > tIdx)
        {
            swap(fIdx, tIdx);
        }

        // Avoid duplicates.
        intPair p(fIdx, tIdx);
        if (pSet.count(p) > 0) continue;

        pSet.insert(p);
        list.push_back(p);
    }

    // Add self edges.
    for (int i = 0; i < size; i++)
    {
        list.push_back(intPair(i, i));
    }

    sortPairsRadix(list);


    // --- Replace indices (in topological order) with vertex-IDs. ---

    // topOrder[i] == vId states that vertex v is at index i in a topological order.
    int topOrder[size];
    makePermutation(topOrder, size);

    for (int i = 0; i < size; i++)
    {
        intPair& p = list[i];

        p.first = topOrder[p.first];
        p.second = topOrder[p.second];
    }

    return list;
}
