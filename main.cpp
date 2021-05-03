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

bool verify(const vector<vector<int>>& testCase, size_t* answer)
{
    int size = testCase.size();


    // --- Check if all indices exist exactly once. ---

    int idxCount[size];
    for (int i = 0; i < size; i++) idxCount[i] = 0;

    for (int i = 0; i < size; i++)
    {
        size_t idx = answer[i];

        if (idx >= size)
        {
            cout << "Answer contains invalid index." << endl;
            return false;
        }

        idxCount[idx]++;
    }

    for (size_t idx = 0; idx < size; idx++)
    {
        if (idxCount[idx] < 1)
        {
            cout << "Answer does not contain index " << idx << "." << endl;
            return false;
        }

        if (idxCount[idx] > 1)
        {
            cout << "Answer contains index " << idx << " too often." << endl;
            return false;
        }
    }


    // --- Check if sorted. ---

    for (int i = 1; i < size; i++)
    {
        size_t smlIdx = answer[i - 1];
        size_t lrgIdx = answer[i];

        const vector<int>& smlVec = testCase[smlIdx];
        const vector<int>& lrgVec = testCase[lrgIdx];

        if (!smallerEqual(smlVec, lrgVec))
        {
            cout << "Answer not sorted correctly." << endl;
            return false;
        }
    }

    return true;
}

int main(int argc, char* argv[])
{
    cout << "*** Union Join and Subset Graph of Acyclic Hypergraphs ***" << endl;


    srand(19082017);

    int totalTestNo = 10000;
    int maxSize = 256;
    int maxLen = 256;
    int maxC = 30;

    for (int testNo = 1, perc = -1; testNo <= totalTestNo; testNo++)
    {
        int size = rand() % min(testNo, maxSize) + 5;

        vector<vector<int>> testCase = buildTestCase
        (
            size,
            rand() % min(testNo, maxLen) + 5,
            rand() % min(testNo, maxC) + 5
        );

        size_t* answer = SubsetGraph::lexSort(testCase.data(), size);
        bool correct = verify(testCase, answer);

        if (!correct)
        {
            cout << "-- Test " << testNo << " --" << endl;

            cout << "Test Case:" << endl;
            for (int sId = 0; sId < size; sId++)
            {
                cout << sId << ":";

                const vector<int>& vec = testCase[sId];
                for (int i = 0; i < vec.size(); i++)
                {
                    cout << " " << vec[i];
                }

                cout << endl;
            }
            cout << endl;


            cout << "Answer:" << endl;
            for (int oIdx = 0; oIdx < size; oIdx++)
            {
                size_t sIdx = answer[oIdx];
                cout << oIdx << "  " << sIdx << ":";

                const vector<int>& vec = testCase[sIdx];
                for (int i = 0; i < vec.size(); i++)
                {
                    cout << " " << vec[i];
                }

                cout << endl;
            }
        }

        delete[] answer;
        if (!correct) break;


        // --- Print progress. ---

        int progress = (testNo * 100) / totalTestNo;

        if (progress > perc)
        {
            perc = progress;

            // Based on https://stackoverflow.com/a/21870633.
            cout << "\r" << perc << " %" << flush;
        }

        if (perc == 100) cout << endl;

    }
}
