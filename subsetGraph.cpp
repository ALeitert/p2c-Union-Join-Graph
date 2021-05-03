#include <stdexcept>

#include "helper.h"
#include "reducedSet.h"
#include "subsetGraph.h"


// Lexicographically sorts the given list of vectors.
// Returns an array A[] such that A[i] is the ID of the vector which is at position i in a lex. order.
size_t* lexSort(const vector<vector<int>> lst);


// Implements a naive approach to find all subset relations (compairs all pairs of hyperedges).
vector<intPair> SubsetGraph::naive(const Hypergraph& hg)
{
    int m = hg.getESize();
    vector<intPair> answer;

    for (int i = 0; i < m; i++)
    {
        const vector<int>& iSet = hg[i];

        for (int j = i + 1; j < m; j++)
        {
            const vector<int>& jSet = hg[j];

            bool iSubsetJ = true;
            bool jSubsetI = true;

            for (int pI = 0, pJ = 0; iSubsetJ || jSubsetI;)
            {
                if (pI >= iSet.size() && pJ >= jSet.size()) break;

                if (pI >= iSet.size())
                {
                    // S_j contains an element not in S_i.
                    jSubsetI = false;
                    break;
                }

                if (pJ >= jSet.size())
                {
                    // S_i contains an element not in S_j.
                    iSubsetJ = false;
                    break;
                }

                int vI = iSet[pI];
                int vJ = jSet[pJ];

                iSubsetJ = iSubsetJ && (vI >= vJ);
                jSubsetI = jSubsetI && (vI <= vJ);

                if (vI <= vJ) pI++;
                if (vI >= vJ) pJ++;
            }

            if (iSubsetJ) answer.push_back(intPair(j, i));
            if (jSubsetI) answer.push_back(intPair(i, j));
        }
    }

    sortPairsRadix(answer);
    return answer;
}


// Implements Pritchard's "simple" algorithm as published in Algorithmica 1999.
vector<intPair> SubsetGraph::pritchardSimple(const Hypergraph& hg)
{
    // Pritchard assumes for their algorithm that there are no two equal sets.
    // For now, we do not address that problem. If needed, we simplify the hypergraph later.

    // We assune the following about the given hypergraph:
    //   - The internal adjacency lists are sorted.
    //   - Each hyperedge contains at least one vertex.
    //   - Lists do not contain duplicates.


    // --- Preliminaries ---

    //     F  The given hypergraph.

    //   F.y  The family of hyperedges x of F such that the hyperedge y is a subset of x.
    //        That is, F.y = { x | x \subseteq y }.

    // F.{d}  The set of all hyperedges x of F such that x contains the vertex d.


    // --- Outline ---

    // 1) Order F, i.e., assign each hyperedge in F a unique index.
    // 2) For each vertex d, compute F.{d}.
    // 3) For each hyperedge y, record edge (x, y) for each x in F.y - y.

    // Steps 1) and 2) are already done in the given hypergraph.


    // --- Step 3) ---

    vector<intPair> result;

    for (int yId = 0; yId < hg.getESize(); yId++)
    {
        const vector<int>& vertices = hg[yId];
        if (vertices.size() <= 0) throw std::invalid_argument("Invalid hypergraph.");

        // Compute F.y using the following relation:
        // F.y = \bigcup_{d \in y} F.{d}


        // Initialise intersection with hyperedges of "first" vertex.
        vector<int> intersection(hg(vertices[0]));

        // Intersect with hyperedges of all other vertices.
        for (int vIdx = 1 /* 0 done above */; vIdx < vertices.size(); vIdx++)
        {
            int vId = vertices[vIdx];
            const vector<int>& vEdges = hg(vId);

            int newSize = 0;
            for (int i = 0, j = 0; i < intersection.size() && j < vEdges.size();)
            {
                int iEdge = intersection[i];
                int jEdge = vEdges[j];

                if (iEdge == jEdge)
                {
                    intersection[newSize] = intersection[i];
                    newSize++;
                }

                if (iEdge <= jEdge) i++;
                if (iEdge >= jEdge) j++;
            }

            intersection.resize(newSize);
        }

        // Intersection calculated. Add edges to result.
        for (int i = 0; i < intersection.size(); i++)
        {
            int xId = intersection[i];
            if (xId == yId) continue;

            result.push_back(intPair(xId, yId));
        }
    }

    sortPairsRadix(result);
    return result;
}

// Implements Pritchard's algorithm with reduced sets.
vector<intPair> SubsetGraph::pritchardReduced(const Hypergraph& hg)
{
    // Pritchard assumes for their algorithm that there are no two equal sets.
    // For now, we do not address that problem. If needed, we simplify the hypergraph later.

    // We assune the following about the given hypergraph:
    //   - The internal adjacency lists are sorted.
    //   - Each hyperedge contains at least one vertex.
    //   - Lists do not contain duplicates.


    // --- Preliminaries ---

    //     F  The given hypergraph.

    //   F.y  The family of hyperedges x of F such that the hyperedge y is a subset of x.
    //        That is, F.y = { x | x \subseteq y }.

    // F.{d}  The set of all hyperedges x of F such that x contains the vertex d.


    // --- Outline ---

    // 1) Order F, i.e., assign each hyperedge in F a unique index.
    // 2) For each vertex d, compute F.{d}.
    // 3) For each hyperedge y, record edge (x, y) for each x in F.y - y.

    // Steps 1) is already done in the given hypergraph.


    // --- Step 2)  Create reduced sets for each vertex. ---

    const int n = hg.getVSize();
    const int m = hg.getESize();

    ReducedSet vSets[n];
    for (int vId = 0; vId < n; vId++)
    {
        vSets[vId] = ReducedSet(hg(vId));
    }


    // --- Step 3) ---

    vector<intPair> result;

    for (int yId = 0; yId < m; yId++)
    {
        const vector<int>& vertices = hg[yId];
        if (vertices.size() <= 0) throw std::invalid_argument("Invalid hypergraph.");

        // Compute F.y using the following relation:
        // F.y = \bigcup_{d \in y} F.{d}


        // Initialise intersection with hyperedges of "first" vertex.
        ReducedSet intersection(vSets[vertices[0]]);

        // Intersect with hyperedges of all other vertices.
        for (int vIdx = 1 /* 0 done above */; vIdx < vertices.size(); vIdx++)
        {
            int vId = vertices[vIdx];
            intersection &= vSets[vId];
        }

        // Intersection calculated. Add edges to result.
        for (auto it = intersection.begin(); it != intersection.end(); ++it)
        {
            int xId = *it;
            if (xId == yId) continue;

            result.push_back(intPair(xId, yId));
        }
    }

    sortPairsRadix(result);
    return result;
}

// Implements Pritchard's algorithm with presorted hyperedges.
vector<intPair> SubsetGraph::pritchardRefinement(const Hypergraph& hg)
{
    throw runtime_error("Not implemented");

    // Pritchard assumes for their algorithm that there are no two equal sets.
    // For now, we do not address that problem. If needed, we simplify the hypergraph later.

    // We assune the following about the given hypergraph:
    //   - The internal adjacency lists are sorted.
    //   - Each hyperedge contains at least one vertex.
    //   - Lists do not contain duplicates.


    // --- Preliminaries ---

    //     F  The given hypergraph.

    //   F.y  The family of hyperedges x of F such that the hyperedge y is a subset of x.
    //        That is, F.y = { x | x \subseteq y }.

    // F.{d}  The set of all hyperedges x of F such that x contains the vertex d.

    //  w(d)  The weight of a vertex d, defined as the number of hyperedges that contain d.



    // --- Outline ---

    // 1) Order F:
    //    1.1 Compute the weight of each domain element.
    //    1.2 Totally order the domain elements by non-increasing weight;
    //        break ties arbitrarily.
    //    1.3 Order the vertices of each hyperedge with respect to their weight.
    //    1.4 Sort the hyperedges lexicographically, using the ordered vertices as the ordered alphabet.


    // 2) For each vertex d, compute F.{d}.
    // 3) For each hyperedge y, record edge (x, y) for each x in F.y - y.


    int n = hg.getVSize();
    int m = hg.getESize();


    // --- Step 1.2: Sort vertices by weight. ---


    int vWeiOrder[n];

    // We use counting sort.
    int count[m];
    for (int i = 0; i < m; i++) count[i] = 0;

    // Count.
    for (int vId = 0; vId < n; vId++)
    {
        int key = hg(vId).size();
        count[key]++;
    }

    // Prefix sums.
    for (int i = 1; i < m; i++)
    {
        count[i] += count[i - 1];
    }

    // Sort.
    for (int vId = n - 1; vId >= 0; vId++)
    {
        int key = hg(vId).size();
        count[key]--;
        int idx = count[key];
        vWeiOrder[idx] = vId;
    }


    // --- Step 1.3: Sort verticies within hyperedges by their weight. ---

    // Two Options
    //   1) Sort with original vertex IDs.
    //   2) Fill hyperedges with index in sorted order instead.
    // For now, we use option 2.


    vector<vector<int>> hyperedges;
    hyperedges.resize(m);

    // Sort into hyperedges.
    for (int voIdx = 0; voIdx < n; voIdx++)
    {
        int vId = vWeiOrder[voIdx];
        const vector<int>& vHypEdges = hg(vId);

        for (int eId : vHypEdges)
        {
            hyperedges[eId].push_back(voIdx); // Option 2.
        }
    }


    // --- Step 1.4: Sort the hyperedges lexicographically. ---

    size_t* eLexOrder = lexSort(hyperedges);


}

// Lexicographically sorts the given list of vectors.
// Returns an array A[] such that A[i] is the ID of the vector which is at position i in a lex. order.
size_t* lexSort(const vector<vector<int>> lst)
{
    // We implement an algorithm presented in [1]. It allows to lexicographically sort
    // strings with n total characters from an alphabet of size m in O(n + m) time.

    // [1] Aho, Hopcroft, Ullman:
    //     The Design and Analysis of Computer Algorithms.
    //     Addison-Wesley, 1974.



    // Phase 1: Determine which characters appear at which position.

    // Step 1.1: Collect all characters and sort them by their value and their position.
    // That is, for each position p (with 1 <= p <= L) and each characters c at position p,
    // create a pair (p, c) and sort all these pairs using radix sort. Note that the
    // base is changing.


    // -- Determine total and maximum length. --

    size_t lstSize = lst.size();
    size_t totalLength = 0;
    size_t maxLength = 0;

    for (int i = 0; i < lstSize; i++)
    {
        totalLength += lst[i].size();
        maxLength = max(maxLength, lst[i].size());
    }


    // -- Build pairs and sort them. --

    vector<intPair> pcPairs;
    for (int i = 0; i < lstSize; i++)
    {
        const vector<int>& str = lst[i];

        for (int p = 0; p < str.size(); p++)
        {
            pcPairs.push_back(intPair(p, str[p]));
        }
    }

    sortPairsRadix(pcPairs);


    // Step 1.2: Remove dublicates and partition by length.

    // In [1], NonEmpty only contais the set of characters that are used for that position.
    // We additionally count how often each appears and then calculte the prefix-sums.

    vector<vector<intPair>> NonEmpty;

    for (int p = 0, i = 0 /* index in list */; p < maxLength; p++) // Note that each p exists.
    {
        NonEmpty.push_back(vector<intPair>());
        vector<intPair>& neVec = NonEmpty[p];


        // Iterate over all characters with same position.
        for (int lastC = -1; i < totalLength && pcPairs[i].first == p; i++)
        {
            int c = pcPairs[i].second;

            if (c != lastC)
            {
                neVec.push_back(intPair(c, 0));
                lastC = c;
            }

            neVec.back().second++;
        }

        // Compute prefix sums.
        for (int j = 1; j < neVec.size(); j++)
        {
            neVec[j].second += neVec[j - 1].second;
        }
    }


    // Phase 2: Raddix sort.

    // Since we sort strings, we start sorting at the last position.
    // Shorter strings are added later and always at the beginning.

    // Note that only sort indices and do not rearange the given array.


    // Step 2.1: Sort by length.

    // Arrays to store orders.
    // No need to initialise, since we never read before writing them completely.
    size_t* orgOrder = new size_t[lstSize];
    size_t* newOrder = new size_t[lstSize];


    // We use counting sort.
    int lenCount[maxLength + 1]; // +1 since indices start at and lengths at 1.
    for (int i = 0; i <= maxLength; i++) lenCount[i] = 0;

    // Count.
    for (size_t sIdx = 0; sIdx < lstSize; sIdx++)
    {
        int key = lst[sIdx].size();
        lenCount[key]++;
    }

    // Prefix sums.
    for (int i = 1; i <= maxLength; i++)
    {
        lenCount[i] += lenCount[i - 1];
    }

    // We need that for later.
    // Makes it easy to determine start and end of each group of lengths.
    int lenRange[maxLength + 1];
    copy(lenCount, lenCount + maxLength + 1, lenRange);

    // Sort.
    for (size_t sIdx = lstSize - 1; sIdx < numeric_limits<size_t>::max(); sIdx--)
    {
        int key = lst[sIdx].size();
        lenCount[key]--;
        int oIdx = lenCount[key];
        orgOrder[oIdx] = sIdx;
    }

    // orgOrder[] now represents the given strings sorted by length.


    // Step 2.2: Sort strings.

    // Allows to simply swap both after each iteration, istead of copying numbers back.
    copy(orgOrder, orgOrder + lstSize, newOrder);


    // It is important that cCount is not cleared inside the loop below.
    // Doing so would result in O(b) extra runtime for each digit and, therefore, in
    // O(n * b) total time. The whole purpose of this approach is to avoid that.
    vector<int> cCount;

    for (int pos = maxLength - 1; pos >= 0; pos--)
    {
        int beg = lenRange[pos];
        int end = lstSize; // exclusive


        // Shortened counting sort:
        // We do not need to count nore compute prefix sums.
        // These values are already stored in NonEmpty;

        // Update counting array.
        vector<intPair>& lenVec = NonEmpty[pos];
        for (const intPair& pair : lenVec)
        {
            const int& c = pair.first;

            if (cCount.size() <= c) cCount.resize(c + 1);
            cCount[c] = pair.second;
        }

        // Sort.
        for (int i = end - 1; i >= beg; i--)
        {
            size_t sIdx = orgOrder[i];
            int chr = lst[sIdx][pos];

            cCount[chr]--;
            int oIdx = cCount[chr] + beg;

            newOrder[oIdx] = sIdx;
        }

        // Swap pointers to do other directin in next iteration.
        swap(orgOrder, newOrder);
    }

    // Lexicographical order is now in orgOrder[] (due to swapping).

    delete[] newOrder;
    return orgOrder;
}
