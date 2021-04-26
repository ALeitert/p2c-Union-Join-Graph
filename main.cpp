#include <algorithm>
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
        if (pSet.count(p) == 0) continue;

        pSet.insert(p);
        list.push_back(p);
    }

    return Hypergraph(list);
}


bool compPair(pair<int, int> p1, pair<int, int> p2)
{
    if (p1.first != p2.first)
    {
        return p1.first < p2.first;
    }

    return p1.second < p2.second;
}

bool testRSort(int testNo)
{
    int maxSize = min(10000, testNo) + 5;
    int size = rand() % maxSize + 5;

    vector<pair<int, int>> list1;
    vector<pair<int, int>> list2;

    for (int i = 0; i < size; i++)
    {
        int f = rand() % maxSize;
        int s = rand() % maxSize;

        list1.push_back(pair<int, int>(f, s));
        list2.push_back(pair<int, int>(f, s));
    }

    sort(list1.begin(), list1.end(), compPair);
    sortPairsRadix(list2);

    bool equal = true;

    for (int i = 0; i < size; i++)
    {
        pair<int, int> p1 = list1[i];
        pair<int, int> p2 = list2[i];

        if (p1.first != p2.first || p1.second != p2.second)
        {
            equal = false;

            cout << "l1: ";
            for (int j = 0; j < size; j++)
            {
                cout << " | " << list1[j].first << " " << list1[j].second;
            }
            cout << endl;

            cout << "l2: ";
            for (int j = 0; j < size; j++)
            {
                cout << " | " << list2[j].first << " " << list2[j].second;
            }
            cout << endl;

            break;
        }
    }

    return equal;
}


int main(int argc, char* argv[])
{
    cout << "*** Union Join and Subset Graph of Acyclic Hypergraphs ***" << endl;


    srand(123456);
    int tests = 10000;
    bool allPassed = true;

    cout << "Testing " << tests << " cases." << endl;

    for (int i = 0; i < tests; i++)
    {
        if (!testRSort(i))
        {
            allPassed = false;
            cout << "Test " << i << " failed." << endl;
            break;
        }
    }

    if (allPassed)
    {
        cout << "All tests passed." << endl;
    }
}
