#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <unordered_map>
#include <unordered_set>

#include "algorithms/sorting.h"
#include "dataStructures/graph.h"
#include "helper.h"
#include "hypergraph.h"

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

Graph randomGraph(size_t size)
{
    unordered_map<sizePair, int, sizePairHash> graph;

    for (size_t vId = 1; vId < size; vId++)
    {
        size_t parId = rand() % vId;
        int weight = rand() % size;

        graph.emplace(sizePair(vId, parId), weight);
    }

    size_t logSize = -1;
    for (size_t s = size; s > 0; s /= 2) logSize++;

    int avgDeg = rand() % (logSize * logSize - 3) + 3;
    int edges = (avgDeg * size) / 2 - size + 1;

    for (int i = 0; i < edges; i++)
    {
        size_t uId = rand() % size;
        size_t vId = rand() % (size - 1);

        // Ensure uId > vId
        if (vId >= uId) vId++;
        if (uId < vId) swap(uId, vId);

        sizePair uvPair(uId, vId);
        if (graph.count(uvPair) > 0) continue;

        int weight = rand() % size;
        graph.emplace(sizePair(uId, vId), weight);
    }

    vector<pair<sizePair, int>> edgeList;
    for (auto& edge : graph)
    {
        edgeList.push_back(pair<sizePair, int>(sizePair(edge.first.first, edge.first.second), edge.second));
    }
    sort(edgeList.begin(), edgeList.end());

    vector<size_t> fList;
    vector<size_t> tList;
    vector<int> wList;

    for (size_t i = 0; i < edgeList.size(); i++)
    {
        fList.push_back(edgeList[i].first.first);
        tList.push_back(edgeList[i].first.second);
        wList.push_back(edgeList[i].second);
    }

    return Graph(fList, tList, wList);
}

bool compareResults(const vector<sizePair>& result1, const vector<sizePair>& result2)
{
    int maxI = max(result1.size(), result2.size());
    for (int i = 0; i < maxI; i++)
    {
        const size_t maxSize = numeric_limits<size_t>::max();
        const sizePair dummy = sizePair(maxSize, maxSize);

        const sizePair &ansP = (i < result1.size() ? result1[i] : dummy);
        const sizePair &solP = (i < result2.size() ? result2[i] : dummy);

        if (ansP < solP)
        {
            cout << "Edge ("
                    << ansP.first << ", " << ansP.second
                    << ") in (1) but not (2)." << endl;
            return false;
        }

        if (ansP > solP)
        {
            cout << "Edge ("
                    << solP.first << ", " << solP.second
                    << ") in (2) but not (1)." << endl;
            return false;
        }

        // Both equal, continue with next.
    }

    return true;
}

void print(const Graph& g)
{
    size_t s = g.size();

    for (size_t uId = 0; uId < s; uId++)
    {
        cout << uId << ":";

        size_t uDeg = g[uId].size();
        for (size_t i = 0; i < uDeg; i++)
        {
            size_t vId = g[uId][i];
            int uvW = g(uId)[i];
            cout << "  |  " << vId << " " << uvW;
        }
        cout << endl;
    }
}

int main(int argc, char* argv[])
{
    cout << "*** Union Join and Subset Graph of Acyclic Hypergraphs ***" << endl;
}
