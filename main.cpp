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

int main(int argc, char* argv[])
{
    cout << "*** Union Join and Subset Graph of Acyclic Hypergraphs ***" << endl;

    srand(123456);
    int testCases = 10000;

    for (int testNo = 1, perc = -1; testNo < testCases; testNo++)
    {
        // --- Create random set. ---

        int maxSize = min(testNo, 1024);
        int size = rand() % maxSize + 1;
        vector<int> set;

        // First number.
        set.push_back(rand() % maxSize);

        for (int i = 0; i < size; i++)
        {
            // Next is previous number plus a non-zero amount.
            // Ensures list is sorted.
            set.push_back(set.back() + rand() % maxSize + 1);
        }


        // --- Create reduced set. ---
        ReducedSet rSet(set);

        // Copy stored numbers into a new list.
        vector<int> itList;
        for (auto it = rSet.begin(); it != rSet.end(); ++it)
        {
            itList.push_back(*it);
        }


        // --- Compare both sets. ---

        for (int i = 0; i < max(set.size(), itList.size()); i++)
        {
            if (i >= set.size())
            {
                cout << itList[i] << " in list but not original set.";
                return 1;
            }

            if (i >= itList.size())
            {
                cout << set[i] << " in original set but not in list.";
                return 1;
            }

            if (set[i] != itList[i])
            {
                cout << "Incorrect number at position " << i << endl;

                cout << " set: "; print(set);
                cout << "list: "; print(itList);

                return 1;
            }
        }


        // --- Print progress. ---

        int progress = (testNo * 100) / testCases;

        if (progress > perc)
        {
            perc = progress;
            cout << perc << " %\r" << flush;
        }

        if (perc == 100) cout << endl;
    }
}
