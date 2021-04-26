#include <iostream>
#include <stdlib.h>
#include <unordered_set>

#include "helper.h"
#include "hypergraph.h"

using namespace std;


Hypergraph randomHG(int n, int m, int N)
{
    unordered_set<evPair, evPairHash> pSet;
    vector<evPair> list;

    for (int i = 0; i < N; i++)
    {
        int eId = rand() % m;
        int vId = rand() % n;

        evPair p(eId, vId);
        if (pSet.count(p) > 0) continue;

        pSet.insert(p);
        list.push_back(p);
    }

    sortPairsRadix(list);
    return Hypergraph(list);
}


int main(int argc, char* argv[])
{
    cout << "*** Union Join and Subset Graph of Acyclic Hypergraphs ***" << endl;
}
