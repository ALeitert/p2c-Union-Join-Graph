#include <iostream>
#include <unordered_set>

#include "helper.h"
#include "hypergraph.h"
#include "reducedSet.h"

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

// Prints an int-vector to the terminal.
void print(const vector<int>& vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        cout << " " << vec[i];
    }
    cout << endl;
}

bool compareSets(const vector<int>& set, const vector<int>& list)
{
    // --- Compare both sets. ---

    for (int i = 0; i < max(set.size(), list.size()); i++)
    {
        if (i >= set.size())
        {
            cout << list[i] << " in list but not original set.";
            return false;
        }

        if (i >= list.size())
        {
            cout << set[i] << " in original set but not in list.";
            return false;
        }

        if (set[i] != list[i])
        {
            cout << "Incorrect number at position " << i << endl;
            return false;
        }
    }

    return true;
}

void vecIntersect(vector<int>& v1, const vector<int>& v2)
{
    int newSize = 0;
    for (int i = 0, j = 0; i < v1.size() && j < v2.size();)
    {
        int iNum = v1[i];
        int jNum = v2[j];

        if (iNum == jNum)
        {
            v1[newSize] = v1[i];
            newSize++;
        }

        if (iNum <= jNum) i++;
        if (iNum >= jNum) j++;
    }

    v1.resize(newSize);
}

int main(int argc, char* argv[])
{
    cout << "*** Union Join and Subset Graph of Acyclic Hypergraphs ***" << endl;

    srand(123456);
    int testCases = 10000;

    vector<int> sets[2];
    ReducedSet rSets[2];

    for (int testNo = 1, perc = -1; testNo <= testCases; testNo++)
    {
        int curIdx = testNo & 1;
        int preIdx = 1 - testNo & 1;


        // --- Create random set. ---

        int maxSize = min(testNo, 1024);
        int size = rand() % maxSize + 1;

        vector<int>& set = sets[curIdx];
        set.clear();


        // First number.
        set.push_back(rand() % maxSize);

        for (int i = 0; i < size; i++)
        {
            // Next is previous number plus a non-zero amount.
            // Ensures list is sorted.
            set.push_back(set.back() + rand() % maxSize + 1);
        }


        // --- Create reduced set. ---

        rSets[curIdx] = ReducedSet(set);
        ReducedSet& rSet = rSets[curIdx];

        // Convert to list.
        vector<int> itList = rSet.toList();


        // --- Compare sets. ---
        if (!compareSets(set, itList))
        {
            rSet.print(cout);
            cout << " set: "; print(set);
            cout << "list: "; print(itList);

            return 1;
        }


        // --- Test intersection. ---

        if (testNo > 1)
        {
            vector<int>& preSet = sets[preIdx];
            ReducedSet& preRSet = rSets[preIdx];

            vecIntersect(preSet, set);
            ReducedSet rsInter = (preRSet & rSet);
            preRSet &= rSet;

            vector<int> rIntList1 = rsInter.toList();
            vector<int> rIntList2 = preRSet.toList();


            // Compare sets.
            if (!compareSets(preSet, rIntList1))
            {
                cout << "*** & operator ***" << endl;

                rsInter.print(cout);
                cout << " set: "; print(preSet);
                cout << "list: "; print(rIntList1);

                return 1;
            }


            // Compare sets.
            if (!compareSets(preSet, rIntList2))
            {
                cout << "*** &= operator ***" << endl;

                preRSet.print(cout);
                cout << " set: "; print(preSet);
                cout << "list: "; print(rIntList2);

                return 1;
            }
        }


        // --- Print progress. ---

        int progress = (testNo * 100) / testCases;

        if (progress > perc)
        {
            perc = progress;
            cout << " %\r" << perc << flush;
        }

        if (perc == 100) cout << endl;
    }
}
