#include <iostream>
#include <stdlib.h>
#include <unordered_set>

#include "helper.h"
#include "hypergraph.h"
#include "subsetGraph.h"

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

    sortPairsRadix(list);
    return Hypergraph(list);
}

vector<vector<int>> buildTestCase(int size, int maxLen, int maxC)
{
    vector<vector<int>> strings;
    strings.resize(size);

    for (int i = 0; i < size; i++)
    {
        vector<int>& str = strings[i];

        int len = rand() % maxLen + 1;
        str.resize(len);

        for (int j = 0; j < len; j++)
        {
            str[j] = rand() % maxC;
        }
    }

    return strings;
}

// Determines if the "left" vector is lexicographically smaller than the "right".
bool smallerEqual(const vector<int>& lhs, const vector<int>& rhs)
{
    int minSize = min(lhs.size(), rhs.size());

    for (int i = 0; i < minSize; i++)
    {
        int l = lhs[i];
        int r = rhs[i];

        if (l < r) return true;
        if (l > r) return false;
    }

    // Both equal so far.
    return lhs.size() <= rhs.size();
}

int main(int argc, char* argv[])
{
    cout << "*** Union Join and Subset Graph of Acyclic Hypergraphs ***" << endl;
}
