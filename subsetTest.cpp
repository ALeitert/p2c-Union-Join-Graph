#include <cassert>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "algorithms/sorting.h"
#include "helper.h"
#include "subsetTest.h"

using namespace std;


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

    Sorting::radixSort(list);


    // --- Replace indices (in topological order) with vertex-IDs. ---

    // topOrder[i] == vId states that vertex v is at index i in a reversed topological order.
    int topOrder[size];
    Sorting::makePermutation(topOrder, size);

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

    Sorting::radixSort(pairs);
    return pairs;
}

// Computes a sorted list of set-vertex pairs based on a given transitive closure.
vector<intPair> buildSets(const vector<intPair>& traClos)
{
    // Since closure is sorted, last entry is largest ID.
    int size = traClos.back().first + 1 /* +1 since IDs start at 0 */;

    // Make copy and sort it if needed.
    vector<intPair> sets(traClos);
    Sorting::ensureSorting(traClos);


    // --- Shuffle vertex IDs. ---

    int idShuffle[size];
    Sorting::makePermutation(idShuffle, size);

    for (int i = 0; i < sets.size(); i++)
    {
        int vIdx = sets[i].second;
        sets[i].second = idShuffle[vIdx];
    }

    Sorting::radixSort(sets);
    return sets;
}

void removeLoops(vector<intPair>& list)
{
    int count = 0;
    for (int i = 0; i < list.size(); i++)
    {
        const intPair& p = list[i];
        if (p.first == p.second) continue;

        list[count] = list[i];
        count++;
    }

    list.resize(count);
}


// **** SubsetTest Class ****

// Generates a random hypergraph of the given size and its subsetgraph.
const Hypergraph& SubsetTest::build(int size)
{
    int logSize = -1;
    for (int s = size; s > 0; s /= 2) logSize++;
    int avgDeg = rand() % max(logSize - 2, 1) + 2 /* at least 2, at most log n */;

    vector<intPair> dag = randomDAG(size, avgDeg);
    /* this. */ solution = transitiveClosure(dag);
    vector<intPair> evPairs = buildSets(solution);
    removeLoops(solution);
    /* this. */ h = Hypergraph(evPairs);

    return h;
}

// Verifies that the given edges represent the subset graph.
bool SubsetTest::verify(const vector<intPair>& ans) const
{
    // Ensure input is sorted.
    vector<intPair>* ptr = Sorting::ensureSorting(ans);
    const vector<intPair>& answer = (ptr == nullptr ? ans : *ptr);

    // Ensure solution is sorted.
    Sorting::ensureSorting(solution);


    // --- Compare answer and solution. ---

    for (int i = 0; i < answer.size(); i++)
    {
        const intPair dummy = MaxIntPair;

        const intPair& ansP = answer[i];
        const intPair& solP = (i < solution.size() ? solution[i] : dummy);

        if (ansP < solP)
        {
            cout << "Edge ("
                 << ansP.first << ", " << ansP.second
                 << ") incorrect." << endl;
            return false;
        }

        if (ansP > solP)
        {
            cout << "Edge ("
                 << solP.first << ", " << solP.second
                 << ") missing." << endl;
            return false;
        }

        // Both equal, continue with next.
    }

    if (ptr != nullptr) delete ptr;

    return true;
}

const vector<intPair>& SubsetTest::getSolution()
{
    return solution;
}


// Tests a given function that computes the subset graph of a given hypergraph.
void testGeneralSSG(ssgFun ssg, int seed, int tests, int maxSize)
{
    srand(seed);

    cout << "Testing Subset Graph Implementation for General Hypergraphs." << endl
         << tests << " test cases with max. size " << maxSize << "." << endl;

    bool allPassed = true;

    for (int i = 1, perc = -1; i <= tests; i++)
    {
        // --- Run test. ---

        SubsetTest sst;
        vector<intPair> answer;

        int size = min(i, rand() % maxSize) + 5;
        const Hypergraph& hg = sst.build(size);

        try
        {
            // Run algorithm.
            answer = ssg(hg);
        }
        catch (const exception& e)
        {
            cout << "Test " << i << " failed with exception." << endl;
            cerr << e.what() << endl;

            allPassed = false;
            break;
        }

        bool correct = sst.verify(answer);


        // Cancel if test case fails.
        if (!correct)
        {
            cout << "Test " << i << " failed." << endl;
            cout << "  answer: "; print(answer);
            cout << "solution: "; print(sst.getSolution());

            hg.print(cout);

            allPassed = false;
            break;
        }


        // --- Print progress. ---

        int progress = (i * 100) / tests;

        if (progress > perc)
        {
            perc = progress;

            // Based on https://stackoverflow.com/a/21870633.
            cout << perc << " %\r" << flush;
        }

        if (perc == 100) cout << endl;
    }

    if (allPassed)
    {
        cout << "All tests passed." << endl;
    }
}
