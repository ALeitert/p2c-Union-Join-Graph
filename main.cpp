#include <iostream>
#include <stdlib.h>
#include <unordered_set>

#include "algorithms/sorting.h"
#include "dataStructures/hypergraph.h"
#include "testing/testing.h"
#include "testing/unionJoinTest.h"
#include "helper.h"

using namespace std;


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

    Sorting::radixSort(list);
    return Hypergraph(list);
}


int main(int argc, char* argv[])
{
    cout << "*** Union Join and Subset Graph of Acyclic Hypergraphs ***" << endl;


    const int seed = 19082017;
    const int tests = 1000;
    const int maxSize = 5000;


    // --- Test if results are equal. ---

    bool equal = Testing::unionJoinGraph
    (
        Testing::UnionJoinTest::ujgLeitertPritchard,
        Testing::UnionJoinTest::ujgLineKruskal,
        seed,
        tests,
        maxSize
    );


    // --- Test runtime. ---

    uint64_t lpTime = Testing::unionJoinGraph
    (
        Testing::UnionJoinTest::ujgLeitertPritchard,
        "Leitert-Pritchard",
        seed,
        tests,
        maxSize
    );

    printTime(lpTime, cout);
    cout << endl;


    uint64_t lkTime = Testing::unionJoinGraph
    (
        Testing::UnionJoinTest::ujgLineKruskal,
        "Line-Kruskal",
        seed,
        tests,
        maxSize
    );

    printTime(lkTime, cout);
    cout << endl;
}
