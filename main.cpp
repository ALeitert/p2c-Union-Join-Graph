#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <unordered_set>

#include "helper.h"
#include "hypergraph.h"
#include "subsetGraph.h"
#include "subsetTest.h"

using namespace std;
using namespace std::chrono;


Hypergraph randomHG(int n, int m, int N)
{
    unordered_set<intPair, intPairHash> pSet;
    vector<intPair> list;

    for (int i = 0; i < N; i++)
    {
        int eId = rand() % m;
        int vId = rand() % n;

        intPair p(eId, vId);
        if (pSet.count(p) > 0) continue;

        pSet.insert(p);
        list.push_back(p);
    }

    sortPairsRadix(list);
    return Hypergraph(list);
}

void runTest(ssgFun ssg, string name)
{
    const int seed = 190082017;
    const int tests = 5000;
    const int maxSize = 2000;

    cout << "\n--- " << name << " ---" << endl;

    auto start = high_resolution_clock::now();
    testGeneralSSG(ssg, seed, tests, maxSize);
    auto end = high_resolution_clock::now();

    printTime(duration_cast<milliseconds>(end - start).count(), cout);
    cout << endl;
}


int main(int argc, char* argv[])
{
    cout << "*** Union Join and Subset Graph of Acyclic Hypergraphs ***" << endl;

    runTest(SubsetGraph::doNothing,           "Reference");
    runTest(SubsetGraph::naive,               "Naive Algorithm");
    runTest(SubsetGraph::pritchardSimple,     "Pritchard's Simple Algorithm");
    runTest(SubsetGraph::pritchardReduced,    "Pritchard's Algorithm with Reduced Sets");
    runTest(SubsetGraph::pritchardRefinement, "Pritchard's Algorithm with Sorting and History");
}
