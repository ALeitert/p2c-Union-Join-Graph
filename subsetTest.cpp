#include <algorithm>
#include <unordered_set>
#include <vector>

#include "helper.h"
#include "subsetTest.h"


// Generates a random DAG of the given size.
// Returns an edge list of the generated graph in topological order.
vector<intPair> randomDAG(int size, int avgDeg)
{
    // --- Create topological order. ---

    // topOrder[i] == vId states that vertex v is at index i in a topological order.
    int topOrder[size];
    for (int i = 0; i < size; i++)
    {
        topOrder[i] = i;
    }

    // No idea why std::begin()/end() do not work.
    random_shuffle(topOrder, topOrder + size);


    // --- Create edges. ---

    unordered_set<intPair, intPairHash> pSet;
    vector<intPair> list;

    int edges = (avgDeg * size) / 2;

    for (int i = 0; i < edges; i++)
    {
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

    sortPairsRadix(list);

    // Replace indices (in topological order) with vertex-IDs.
    for (int i = 0; i < size; i++)
    {
        intPair& p = list[i];

        p.first = topOrder[p.first];
        p.second = topOrder[p.second];
    }

    return list;
}
