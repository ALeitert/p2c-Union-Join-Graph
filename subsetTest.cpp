#include <algorithm>
#include <unordered_set>
#include <vector>

#include "helper.h"
#include "subsetTest.h"


// Generates a random DAG of the given size.
// Returns an sorted edge list of the generated graph.
vector<intPair> randomDAG(int size, int avgDeg)
{
    // --- Create topological order. ---

    // topOrder[vId] == i states that vertex v is at index i in a topological order.
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
        int frId = rand() % size;

        // Ensures vId != uId
        int toId = rand() % (size - 1);
        if (toId >= frId) toId++;

        if (topOrder[frId] > topOrder[toId])
        {
            swap(frId, toId);
        }

        // Avoid duplicates.
        intPair p(frId, toId);
        if (pSet.count(p) > 0) continue;

        pSet.insert(p);
        list.push_back(p);
    }

    sortPairsRadix(list);
    return list;
}
