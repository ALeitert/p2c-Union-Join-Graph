#include <cassert>
#include <unordered_set>
#include <vector>

#include "helper.h"
#include "subsetTest.h"


// Generates a random DAG of the given size.
// Returns an edge list of the generated graph in reversed topological order.
// Each vertex also contains an edge to itself (allows to identify isolated vertices).
vector<intPair> randomDAG(int size, int maxAvgDeg)
{
    // --- Create edges. ---

    unordered_set<intPair, intPairHash> pSet;
    vector<intPair> list;

    int maxEdges = (maxAvgDeg * size) / 2;
    int edges = rand() % (maxEdges + 1); // +1 because modolu is exclusive.

    for (int i = 0; i < edges; i++)
    {
        // fIdx and tIdx are indices in reversed topological order.
        // IDs are assigned later.

        int fIdx = rand() % size;

        // Ensures frId != tIdx
        int tIdx = rand() % (size - 1);
        if (tIdx >= fIdx) tIdx++;

        // Ensure it is reversed topological order.
        if (fIdx < tIdx)
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

    // topOrder[i] == vId states that vertex v is at index i in a reversed topological order.
    int topOrder[size];
    makePermutation(topOrder, size);

    for (int i = 0; i < list.size(); i++)
    {
        intPair& p = list[i];

        p.first = topOrder[p.first];
        p.second = topOrder[p.second];
    }

    return list;
}

// Computes the transitive closure of a given DAG.
// Assumes that the DAG is given as edge list in topological order and that each vertex has a loop.
// Returns a sorted list of edges.
vector<intPair> transitiveClosure(const vector<intPair>& dag)
{
    vector<unordered_set<int>> allSets;

    for (int i = 0; i < dag.size(); i++)
    {
        const intPair& p = dag[i];

        int frId = p.first;
        int toId = p.second;

        while (allSets.size() <= frId)
        {
            // Adding allSets.size() ensures that set contains itself.
            // (int) prevents warning (due to coversion from size_t).
            allSets.push_back(unordered_set<int>({ (int)allSets.size() }));
        }

        if (frId == toId) continue;

        assert(frId < allSets.size());
        assert(toId < allSets.size());
        unordered_set<int>& set = allSets[frId];
        unordered_set<int>& subSet = allSets[toId];

        // Add all vertices from subset into current set.
        for (const int& vId : subSet)
        {
            if (set.count(vId) == 0) set.insert(vId);
        }
    }


    // --- Convert to edge list (of set-to-set edges). ---

    vector<intPair> pairs;
    for (int i = 0; i < allSets.size(); i++)
    {
        unordered_set<int>& set = allSets[i];

        for (auto it = set.begin(); it != set.end(); it++)
        {
            pairs.push_back(intPair(i, *it));
        }
    }

    sortPairsRadix(pairs);
    return pairs;
}

// Computes a sorted list of set-vertex pairs based on a given transitive closure.
// Assumes that the closure is sorted.
vector<intPair> buildSets(const vector<intPair>& traClos)
{
    // Since closure is sorted, last entry is largest ID.
    int size = traClos.back().first + 1 /* +1 since IDs start at 0 */;

    // Make copy.
    vector<intPair> sets(traClos);


    // --- Shuffle vertex IDs. ---

    int idShuffle[size];
    makePermutation(idShuffle, size);

    for (int i = 0; i < sets.size(); i++)
    {
        int vIdx = sets[i].second;
        sets[i].second = idShuffle[vIdx];
    }

    sortPairsRadix(sets);
    return sets;
}
