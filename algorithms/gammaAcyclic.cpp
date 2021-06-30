#include "alphaAcyclic.h"
#include "gammaAcyclic.h"
#include "sorting.h"


// Genrates a gamma-acyclic hypergraph with n vertices and m hyperedges.
Hypergraph GammaAcyclic::genrate(size_t m, size_t n)
{
    // Adjacency lists.
    vector<vector<int>> vList(n);
    vector<vector<int>> eList(m);

    // Available vertices and hyperedges.
    vector<int> allV(n);
    vector<int> allE(m);

    size_t vCount = 0;
    size_t eCount = 0;
    size_t N = 0;

    Sorting::makePermutation(allV.data(), n);
    Sorting::makePermutation(allE.data(), m);


    // --- Create first vertex-hyperedge pair. ---

    // First pair.
    int v0 = allV[vCount];
    int e0 = allE[eCount];

    // "Remove" from list.
    vCount++;
    eCount++;
    N++;

    // Add to hypergraph.
    vList[v0].push_back(e0);
    eList[e0].push_back(v0);


    // --- Generate remaining hypergraph. ---

    for (size_t total = n + m - (vCount + eCount); total > 0; total--)
    {
        size_t rnd = rand() % total;
        size_t par = rand() % (vCount + eCount);

        bool newIsV = rnd < n - vCount;
        bool parIsV = par < vCount;

        if (newIsV && parIsV)
        {
            // New vertex is false twin (of existing vertex).

            int vId = allV[vCount];
            int pId = allV[par];

            // Copy set of hyperedges.
            vList[vId] = vList[pId];

            // Add vertex to hyperedges.
            for (const int& eId : vList[vId])
            {
                eList[eId].push_back(vId);
            }

            vCount++;
            N += vList[vId].size();
        }

        if (newIsV && !parIsV)
        {
            // New vertex is leaf (adjacent to hyperedge).

            int vId = allV[vCount];
            int eId = allE[par - vCount];

            // Make vertex adjecent to hyperedge.
            vList[vId].push_back(eId);
            eList[eId].push_back(vId);

            vCount++;
            N++;
        }

        if (!newIsV && parIsV)
        {
            // New hyperedge is leaf (adjacent to vertex).

            int eId = allE[eCount];
            int vId = allV[par];

            // Make vertex adjecent to hyperedge.
            vList[vId].push_back(eId);
            eList[eId].push_back(vId);

            eCount++;
            N++;
        }

        if (!newIsV && !parIsV)
        {
            // New hyperedge is false twin (of existing hyperedge).

            int eId = allE[eCount];
            int pId = allE[par - vCount];

            // Copy set of vertices.
            eList[eId] = eList[pId];

            // Add hyperedge to vertices.
            for (const int& vId : eList[eId])
            {
                vList[vId].push_back(eId);
            }

            eCount++;
            N += eList[eId].size();
        }
    }

    vector<intPair> hg;
    hg.reserve(N);

    for (int eId = 0; eId < m; eId++)
    {
        const vector<int>& vSet = eList[eId];

        for (const int& vId : vSet)
        {
            hg.push_back(intPair(eId, vId));
        }
    }

    return Hypergraph(hg);
}

// Computes the union join graph for a given gamma-acyclic hypergraph.
Graph GammaAcyclic::unionJoinGraph(const Hypergraph& hg)
{
    // Lemma: An acyclic hypergraph is γ-acyclic if and only if its line graph
    //        is isomorphic to its union join graph.

    // Not that the dual hypergraph H* of a gamma-acyclic hypergraph H is gamma-
    // We can, therefore, compute L(H) = 2Sec(H*) in linear time by using the
    // join tree of H*.


    const Hypergraph& dual = hg.getDual();

    const size_t n = dual.getVSize();
    const size_t m = dual.getESize();


    // --- Compute join tree and DFS on it. ---

    vector<int> dualJoinTree = AlphaAcyclic::getJoinTree(dual);

    // Determine root.
    // The used join tree algorithm normally uses the largest ID as root.
    // Hence we chack from the largest to thesmallest ID.
    int rootId = -1;
    for (rootId = m - 1; rootId >= 0 && dualJoinTree[rootId] >= 0; rootId--) { }

    // Run DFS.
    vector<size_t> dfs = AlphaAcyclic::joinTreeDfs(dualJoinTree, rootId).first;

    // The computed DFS states, for a given node-ID, the index of that node in
    // the pre-order of the tree.
    // The following allows us to determine an ID based on an index.

    vector<int> preOrder(m);

    for (int eId = 0; eId < m; eId++)
    {
        size_t eIdx = dfs[eId];
        preOrder[eIdx] = eId;
    }


    // --- Compute 2-Section graph. ---

    vector<intPair> edgeList;

    vector<int> oldIds;
    vector<int> newIds;
    vector<bool> flagged(n, false);

    for (size_t i = 0; i < m; i++)
    {
        int eId = preOrder[i];
        const vector<int>& vList = dual[eId];


        // Partition into new (not flagged) and old (flagged) vertices.
        for (const int& vId : vList)
        {
            if (flagged[vId])
            {
                oldIds.push_back(vId);
            }
            else
            {
                newIds.push_back(vId);
                flagged[vId] = true;
            }
        }

        // Connect new vertices with old vertices.
        for (const int& nId : newIds)
        {
            for (const int& oId : oldIds)
            {
                edgeList.push_back(intPair(oId, nId));
            }

            oldIds.push_back(nId);
        }

        oldIds.clear();
        newIds.clear();
    }


    // --- Process edges to create graph. ---

    // Make sure first ID is larger.
    for (intPair& edge : edgeList)
    {
        if (edge.first < edge.second)
        {
            swap(edge.first, edge.second);
        }
    }

    Sorting::radixSort(edgeList);
    return Graph(edgeList, vector<int>(edgeList.size(), 0) /* weights */);
}


// Anonymous namespace with helper functions for prune().
namespace
{
    typedef pair<vector<size_t>, vector<size_t>> listPair;

    // Computes distances in the incidence graph of a given hypergraph.
    listPair bfs(const Hypergraph& h, int sId, bool onVertex)
    {
        const size_t n = h.getVSize();
        const size_t m = h.getESize();


        // --- Initialise search. ---

        vector<int> vQ;
        vector<int> eQ;

        vector<bool> vInQueue(n, false);
        vector<bool> eInQueue(m, false);

        pair<vector<size_t>, vector<size_t>> result;
        vector<size_t>& vDist = result.first;
        vector<size_t>& eDist = result.second;

        vDist.resize(n, -1);
        eDist.resize(m, -1);

        // Set start vertex.
        if (onVertex)
        {
            vQ.push_back(sId);
            vInQueue[sId] = true;
        }
        else
        {
            eQ.push_back(sId);
            eInQueue[sId] = true;
        }


        // --- Run BFS. ---

        for (size_t dist = 0; ; dist++, onVertex = !onVertex)
        {
            vector<int>& currQ = onVertex ? vQ : eQ;
            vector<int>& nextQ = onVertex ? eQ : vQ;

            vector<bool>& inQueue = onVertex ? eInQueue : vInQueue;
            vector<size_t>& dList = onVertex ? vDist : eDist;

            // End search if everything processed.
            if (currQ.size() == 0) break;

            nextQ.clear();

            for (const int& xId : currQ)
            {
                dList[xId] = dist;

                const vector<int>& xNeighs = onVertex ? h(xId) : h[xId];

                for (const int& yId : xNeighs)
                {
                    if (inQueue[yId]) continue;

                    nextQ.push_back(yId);
                    inQueue[yId] = true;
                }
            }
        }

        return result;
    }

    // Determine the number of lower neighbours for each vertex of the incidence graph.
    listPair getInnerDegrees
    (
        const Hypergraph& h,
        const listPair& id2Layer
    )
    {
        const size_t n = h.getVSize();
        const size_t m = h.getESize();

        const vector<size_t>& v2Layer = id2Layer.first;
        const vector<size_t>& e2Layer = id2Layer.second;

        listPair innerDegrees;
        vector<size_t>& vDegrees = innerDegrees.first;
        vector<size_t>& eDegrees = innerDegrees.second;

        vDegrees.resize(n, 0);
        eDegrees.resize(m, 0);

        // Degrees of vertices.
        for (int vId = 0; vId < n; vId++)
        {
            size_t vLayer = v2Layer[vId];
            size_t& vCount = vDegrees[vId];

            for (const int& eId : h(vId))
            {
                // Neighbour is in a lower layer?
                if (e2Layer[eId] < vLayer) vCount++;
            }
        }

        // Degrees of hyperedges.
        for (int eId = 0; eId < m; eId++)
        {
            size_t eLayer = e2Layer[eId];
            size_t& eCount = eDegrees[eId];

            for (const int& vId : h[eId])
            {
                // Neighbour is in a lower layer?
                if (v2Layer[vId] < eLayer) eCount++;
            }
        }

        return innerDegrees;
    }

    // Sorts vertices within layers by inner degree of vertices.
    // Does not change the given layers and returns a new list of layers instead.
    vector<vector<int>> sortByDegree
    (
        const vector<size_t>& id2Layer,
        const vector<size_t>& innerDegree
    )
    {
        const size_t k = id2Layer.size();


        // --- Counting sort. ---

        // Result of sorting.
        vector<int> byDegree(k);

        // Counter for counting sort.
        vector<size_t> counter;


        // Count.
        for (int xId = 0; xId < k; xId++)
        {
            size_t key = innerDegree[xId];
            if (key >= counter.size()) counter.resize(key + 1);
            counter[key]++;
        }

        // Pre-fix sums.
        for (size_t i = 1; i < counter.size(); i++)
        {
            counter[i] += counter[i - 1];
        }

        // Sort.
        for (int xId = k - 1; xId >= 0; xId--)
        {
            size_t key = innerDegree[xId];

            counter[key]--;
            size_t idx = counter[key];

            byDegree[idx] = xId;
        }


        // -- Rebuild layers. --

        vector<vector<int>> layers;

        // Add vertices into layers again.
        for (size_t idx = 0; idx < k; idx++)
        {
            int xId = byDegree[idx];
            size_t xLayer = id2Layer[xId] >> 1;

            if (xLayer >= layers.size()) layers.resize(xLayer + 1);
            layers[xLayer].push_back(xId);
        }

        return layers;
    }

    // Contracts a subgraph induced by the given vertices and adds its pruning
    // sequence to the given list.
    // Return the ID into which the subgraph is contracted.
    int contractSG(const vector<int> xList, vector<DistH::Pruning>& result, int mod)
    {
        // Check trival cases.
        // Needed to avoid errors.
        if (xList.size() == 0) return -1;
        if (xList.size() == 1) return xList[0];


        // Since we process hypergraphs, the subgraph has no edges. Therefore,
        // each pruneing operation is a FalseTwin operation.

        for (size_t i = 1; i < xList.size(); i++)
        {
            const int& xId = xList[i - 1];
            const int& pId = xList[i];
            const DistH::PruningType ft = DistH::PruningType::FalseTwin;

            result.push_back(DistH::Pruning(xId + mod, ft, pId + mod));
        }

        return xList.back();
    }

    // Helper function for checkTwins().
    // Determines the next neighbour that was not removed.
    // Returns true if the potential twin was found.
    void findNextNeigh
    (
        size_t& idx, // The index to start at.
        size_t& pre, // The index of the pointer to entries.
        const vector<bool>& ignore, // The removed vertices.
        const vector<int>& neigh, // The neighbourhood to search through.
        vector<size_t>& next // Pointers to next entry.
    )
    {
        for
        (
            ;
            idx < neigh.size();
            idx = next[idx + 1] /* cur := cur.next */
        )
        {
            int nId = neigh[idx];

            if (!ignore[nId]) break;

            // "Delete" current node.
            // pre.next = cur.next
            next[pre] = next[idx + 1];
        }
    }

    // Determines if two vertices are twins by comparing their neighbourhoods.
    bool checkTwins
    (
        const vector<int>& uNei,
        const vector<int>& vNei,
        vector<size_t>& uNext,
        vector<size_t>& vNext,
        const vector<bool>& ignore
    )
    {
        for
        (
            size_t

            // Pointer to (pointer to) nodes.
            pI = 0, pJ = 0,

            // First nodes.
            // cur := pre.next
            i = uNext[pI], j = vNext[pJ];

            /* no break condition */;

            // pre := cur
            pI = i + 1, pJ = j + 1,

            // cur := cur.next
            i = uNext[i + 1],
            j = vNext[j + 1]
        )
        {
            // --- Skip ignored nodes and "delete" them. ---

            findNextNeigh(i, pI, ignore, uNei, uNext);
            findNextNeigh(j, pJ, ignore, vNei, vNext);


            // --- Compare neighbours. ---

            // Now, i and j either refer to a neighbour still in G or to
            // the end of their respective neighbourhoods.


            bool iInRange = i < uNei.size();
            bool jInRange = j < vNei.size();


            // If one neighbourhood still contains vertices while the other is
            // completed: not twins.
            if (iInRange != jInRange) return false;

            // Both neighbourhoods completed?
            if (!iInRange) return true;

            // Both neighbours the same?
            if (uNei[i] != vNei[j]) return false;
        }
    }

    // Verifies if a given pruning sequence fits the given graph.
    bool verifySequence(const Hypergraph& h, const vector<DistH::Pruning>& seq)
    {
        // --- Algorithm 4 from [1] ---

        //  1  For j := n - 1 DownTo 1
        //  2      Let xQy = S_j.
        //  3      If Q = P Then
        //  4          If |N(x)| != 1 or xy not in E, Return False.
        //  5      Else
        //  6          If N(x) \cap { x1, ..., x_{j - 1} } !=
        //             N(y) \cap { x1, ..., x_{j - 1} }, Return False.
        //  7  Return True.

        // The approach above verifies the sequence as constructing sequence.
        // We do it the other was as eliminating sequence.


        const size_t n = h.getVSize();
        const size_t m = h.getESize();


        // --- Preprocessing ---

        // Trivally false?
        if (seq.size() != n + m) return false;

        // States if a vertex was removed from H.
        vector<bool> vRemoved(n, false);
        vector<bool> eRemoved(m, false);

        // States for each entry in a neighbourhodd which is the next entry that has
        // not been removed. Allows to efficiently skip these vertices.
        // The first entry in the list states the index of the starting neighbour.
        vector<vector<size_t>> nextLst(n + m);
        for (int xId = 0; xId < n + m; xId++)
        {
            const vector<int>& xNeigh = xId < n ? h(xId) : h[xId - n];

            vector<size_t>& xList = nextLst[xId];
            xList.resize(xNeigh.size() + 1, 0);

            for (size_t idx = 0; idx < xList.size(); idx++)
            {
                xList[idx] = idx;
            }
        }


        // --- Verify sequence. ---

        for (size_t i = 0; i < n + m - 1 /* ignore last */; i++)
        {
            // --- Line 2 ---

            const DistH::Pruning prun = seq[i];

            const int xId = prun.vertex;
            const int yId = prun.parent;
            const DistH::PruningType Q = prun.type;

            const bool xIsVer = xId < n;
            const vector<int>& xNeigh = xIsVer ? h(xId) : h[xId - n];
            vector<size_t>& xNext = nextLst[xId];

            vector<bool>& nRemoved = xIsVer ? eRemoved : vRemoved;
            vector<bool>& xRemoved = xIsVer ? vRemoved : eRemoved;

            const int nMod = xIsVer ? n : 0;

            // --- Line 3 ---

            if (Q == DistH::PruningType::Pendant)
            {
                // --- Line 4 ---

                // Check if x has only y as neighbour.

                bool adjToY = false;
                size_t nCount = 0;

                for
                (
                    size_t idx = xNext[0]; // cur := first
                    idx < xNeigh.size();
                    idx = xNext[idx + 1] // cur := cur.next
                )
                {
                    int nId = xNeigh[idx];
                    if (nRemoved[nId]) continue;

                    // Neighbour found.
                    nCount++;
                    adjToY |= nId + nMod == yId;
                }

                if (nCount != 1 || !adjToY) return false;
            }
            else
            {
                // --- Line 6 ---

                bool twinsInH = checkTwins
                (
                    xNeigh,
                    xIsVer ? h(yId) : h[yId - n],
                    xNext,
                    nextLst[yId],
                    nRemoved
                );
                bool twinsInS = Q == DistH::PruningType::FalseTwin;

                if (!twinsInH || !twinsInS) return false;
            }

            // xQy is correct. Remove x from graph.
            xRemoved[xId - (xIsVer ? 0 : n)] = true;
        }

        // All operations in sequence correct.
        return true;
    }
}

// Computes a pruning sequence for a given gamma-acyclic hypergraph.
// Returns an empty list if the given hypergraph is not gamma-acyclic.
vector<DistH::Pruning> GammaAcyclic::pruningSequence(const Hypergraph& h)
{
    // The implementation is based on the algorithm for distance-hereditary
    // graphs. We were able to simplify various aspects of it since incidence
    // graphs are bipartite.

    //  2  Compute the distance layout < L_1, ..., L_k > from an arbitrary
    //     vertex v.
    //  3  For i := k DownTo 1

    //  8      Sort the vertices of G[L_i] by increasing inner degree.

    // 13      For Each x in L_i taken in increasing inner degree order
    // 14          Set y := PruneCograph(G[N_{i - 1}(x)], j).
    // 15          Contract N_{i - 1}(x) into y.
    // 16          Set j := j + |N_{i - 1}(x)| - 1
    // 17          Set sigma(j) := x, s_j := (xPy), and j := j + 1.

    // Note on lines 8, 9, and 13: The "inner degree" of a vertex is the number
    // of neighbours the vertex has in the layer below in the original graph.


    const size_t n = h.getVSize();
    const size_t m = h.getESize();

    // States if a vertex or hyperedge was "removed" by contracting vertices.
    vector<bool> vIgnore(n, false);
    vector<bool> eIgnore(m, false);

    // The resulting sequence.
    vector<DistH::Pruning> result;


    // --- Line 2 ---

    bool onV = true;
    const int startId = 0;

    const listPair id2Layer = bfs(h, startId, true);
    const vector<size_t>& v2Layer = id2Layer.first;
    const vector<size_t>& e2Layer = id2Layer.second;


    // --- Sort vertices and hyperedges in layers by their inner degree. ---

    // Determine the inner degree of all vertices and hyperedges.
    const listPair inDegrees = getInnerDegrees(h, id2Layer);
    const vector<size_t>& vDegrees = inDegrees.first;
    const vector<size_t>& eDegrees = inDegrees.second;

    vector<vector<int>> layers;

    // Sort by degree into layers.
    {
        vector<vector<int>> sortedVLayers = sortByDegree(v2Layer, vDegrees);
        vector<vector<int>> sortedELayers = sortByDegree(e2Layer, eDegrees);

        vector<vector<int>>& evnLayers = onV ? sortedVLayers : sortedELayers;
        vector<vector<int>>& oddLayers = onV ? sortedELayers : sortedVLayers;

        layers.resize(oddLayers.size() + evnLayers.size());

        for (size_t l = 0; l < layers.size(); l++)
        {
            vector<vector<int>>& lLay = ((l & 1) == 0) ? evnLayers : oddLayers;
            layers[l] = move(lLay[l >> 1]);
        }
    }

    const size_t k = layers.size();


    // --- Line 3 ---

    // We skip layer 0 and treat it later as special case.
    for (size_t i = k - 1; i > 0; i--)
    {
        const vector<int>& iLayer = layers[i];

        // Layer is a vertex-layer
        //     iff (onV and even) || (!onV and !even)
        //     iff onV == even
        const bool isVLayer = onV == ((i & 1) == 0);

        vector<bool>& currIgnore = isVLayer ? vIgnore : eIgnore;
        vector<bool>& downIgnore = isVLayer ? eIgnore : vIgnore;

        const vector<size_t>& downId2Layer  = isVLayer ? e2Layer : v2Layer;

        const int currMod = isVLayer ? 0 : n;
        const int downMod = isVLayer ? n : 0;


        // --- Line 8 ---

        // Vertices in layers are already sorted by degree.


        // --- Lines 9 and 13 ---

        for (size_t degIdx = 0; degIdx < iLayer.size(); degIdx++)
        {
            int xId = iLayer[degIdx];
            if (currIgnore[xId]) continue;


            // --- Determine downwards neighbourhood. ---

            const vector<int>& xNeighs = isVLayer ? h(xId) : h[xId];

            vector<int> xDownN;
            for (size_t dIdx = 0; dIdx < xNeighs.size(); dIdx++)
            {
                int dId = xNeighs[dIdx];
                if (downIgnore[dId] || downId2Layer[dId] >= i) continue;

                xDownN.push_back(dId);
            }


            // --- Lines 14 - 16 ---

            int yId = contractSG(xDownN, result, downMod);

            // "Remove" vertices from graph.
            for (const int& dId : xDownN)
            {
                downIgnore[dId] = dId != yId;
            }


            // --- Lines 10, 11, and 17 ---

            result.push_back
            (
                DistH::Pruning
                (
                    xId + currMod,
                    DistH::PruningType::Pendant,
                    yId + downMod
                )
            );
            currIgnore[xId] = true; // No real need for it, but we do it just to be safe.
        }
    }


    // Layer 0: start vertex of BFS
    result.push_back
    (
        DistH::Pruning
        (
            startId + (onV ? 0 : n),
            DistH::PruningType::Pendant,
            -1
        )
    );

    // Verification of produced sequence
    return verifySequence(h, result) ? result : vector<DistH::Pruning>();
}


// Anonymous namespace with helper functions for bachman().
namespace
{
    class Bachman
    {
    public:

        // Default constructor.
        // Creates an empty data structure.
        Bachman() = default;

        // Constructor.
        // Creates an "empty" diagram for a hypergraph with m hyperedges and n vertices.
        Bachman(size_t m, size_t n) :
            phi(vector<intPair>(m, intPair(-1, -1))),
            psi(vector<intPair>(n, intPair(-1, -1)))
        {
            Phi.reserve(m);
            Psi.reserve(m);
        }


        // Crates a new node in the diagram and returns its ID.
        int createNode()
        {
            int id = adjIn.size();

            adjIn.push_back(vector<int>());
            adjOut.push_back(vector<int>());
            Phi.push_back(vector<int>());
            Psi.push_back(vector<int>());

            return id;
        }

        // Adds a new edge from X to Y into the diagram.
        void addEdge(int xId, int yId)
        {
            adjOut[xId].push_back(yId);
            adjIn[yId].push_back(xId);
        }


        // Determines the node accociated with a given hyperedge.
        int operator[](int eId) const
        {
            return phi[eId].first;
        }

        // Determines the node accociated with a given vertex.
        int operator()(int vId) const
        {
            return psi[vId].first;
        }

        // Updates for a give hyperedge to which node it belongs.
        void setPhi(int eId, int xId)
        {
            setAssignment(eId, xId, phi, Phi);
        }

        // Updates for a give vertex to which node it belongs.
        void setPsi(int vId, int xId)
        {
            setAssignment(vId, xId, psi, Psi);
        }


        // Returns the number of hyperedges accociated with the given node.
        size_t PhiSize(int xId) const
        {
            return Phi[xId].size();
        }

        // Returns the number of vertices accociated with the given node.
        size_t PsiSize(int xId) const
        {
            return Psi[xId].size();
        }

        // Returns the number of incoming edges for the given node.
        size_t inDegree(int xId) const
        {
            return adjIn[xId].size();
        }

        // Returns the number of outgoing edges for the given node.
        size_t outDegree(int xId) const
        {
            return adjOut[xId].size();
        }


        // Determines all hyperedges which can reach the given hyperedge.
        vector<int> canReach(int eId) const
        {
            // Implements line 4 of Algorithm 5.

            int xId = phi[eId].first;


            // --- BFS: Determine nodes Y that can reach X. ---

            vector<int> yList = { xId };
            for (size_t qIdx = 0; qIdx < yList.size(); qIdx++)
            {
                int yId = yList[qIdx];

                for (int zId : adjIn[yId])
                {
                    yList.push_back(zId);
                }
            }


            // --- Determine hyperedges associated with the Y-nodes. ---

            vector<int> result;
            for (int yId : yList)
            {
                for (int epId : Phi[yId])
                {
                    if (epId == eId) continue;
                    result.push_back(epId);
                }
            }

            return result;
        }


    private:

        // Genralised function to updes the assignment of a hyperedge or vertex to a node.
        void setAssignment(int id, int xId, vector<intPair>& f, vector<vector<int>>& F)
        {
            intPair& info = f[id];

            // Element assigned to a node?
            if (info.first >= 0)
            {
                vector<int>& node = F[info.first];

                // "Swap" with last in node.
                int last = node.back();
                node[info.second] = last;
                f[last].second = info.second;

                // Remove from node.
                node.pop_back();
                info.first = -1;
                info.second = -1;
            }

            // -- Add to "new" node. --

            vector<int>& X = F[xId];

            info.first = xId;
            info.second = X.size();

            X.push_back(id);
        }

        // Adjacency list of the Bachman diagram.
        vector<vector<int>> adjIn;  // Incoming edges.
        vector<vector<int>> adjOut; // Outgoing edges.

        // Stores the hyperedges E with phi(E) = X for each X of B.
        vector<vector<int>> Phi;

        // Stores the vertices (of H) in X for each X of B.
        vector<vector<int>> Psi;

        // The functions phi and psi.
        // Stores the node X they map on (first) and where the hyperedge or vertex
        // is stored in Psi or Phi of X, respectively.
        vector<intPair> phi;
        vector<intPair> psi;
    };
}

// Computes a (simplified) Bachman diagram for the given gamma-acyclic hypergraph.
Bachman bachman(const Hypergraph& h)
{
    // --- Algorithm 4 ---

    // -- Notation:
    //    phi(E)   The node of B which represents E.
    //    Phi(X)   The set of hyperedges E such that phi(E) = X.
    //    psi(v)   The node of B which contains v.


    //  1  Compute a pruning sequence sigma = < x_1, x_2, ..., x_{n + m} >
    //     for I(H).

    //  2  Create a new empty graph B.

    //  3  Let x_1 and x_2 represent the vertex v and hyperedge E of H. Create a
    //     new set X = { v }, add it as node to B, and set phi(E) := X and
    //     psi(v) := X.

    //  4  For i := 3 To n + m

    //  5      If x_i represents a vertex v and is a false twin Then
    //  6          Let u be the vertex represented by a twin of x_i and
    //             let X = \psi(u).
    //  7          Add v into X, i.e., set psi(v) := X and X := X \cup { v }.

    //  8      If x_i represents a hyperedge E and is a false twin Then
    //  9          Let E' be the hyperedge represented by a twin of x_i.
    // 10          Set phi(E) := phi(E').

    // 11      If x_i represents a vertex v and is pendant Then
    // 12          Let E be the hyperedge represented by the neighbour of~x_i
    //             and let X = phi(E).
    // 13          If |Phi(X)| = 1 and X has no incoming edges in B Then
    // 14              Add v into X, i.e., set psi(v) := X and X := X \cup { v }.
    // 15          Else
    // 16              Create a new set Y = { v }, add it as node to B, set
    //                 psi(v) := Y and phi(E) := Y, and add the edges (Y, X)
    //                 into B.

    // 17      If x_i represents a hyperedge E and is pendant Then
    // 18          Let v be the vertex represented by the neighbour of x_i and
    //             let X = psi(v).
    // 19          If |X| = 1 and X has no outgoing edges in B Then
    // 20              Set phi(E) := X.
    // 21          Else
    // 22              Create a new set Y = { v }, add it as node into B, set
    //                 X := X \ { v }, set psi(v) := Y and phi(E) := Y, and add
    //                 the edge (X, Y) into B.


    const size_t n = h.getVSize();
    const size_t m = h.getESize();


    // --- Line 1 ---

    vector<DistH::Pruning> sigma = GammaAcyclic::pruningSequence(h);

    // Is H gamma-acyclic?
    if (sigma.size() == 0) return Bachman();

    // Note: pruningSequence() returns an elimination order. Algorithm 4 above
    //       asume a constructing order. We therefore process sigma backwards.


    // --- Line 2 ---

    // Adjacency list of the Bachman diagram.
    Bachman B(m, n);


    // --- Line 3 ---

    {
        // Ignore "first" entry.
        sigma.pop_back();

        DistH::Pruning prun = sigma.back();
        sigma.pop_back();

        int x1 = prun.parent;
        int x2 = prun.vertex;

        int vId = x1 < n ? x1 : x2;
        int eId = (x1 < n ? x2 : x1) - n;

        // Create new node X.
        int X = B.createNode();
        B.setPhi(eId, X);
        B.setPsi(vId, X);
    }


    // --- Line 4 ---

    for (; sigma.size() > 0; sigma.pop_back())
    {
        DistH::Pruning prun = sigma.back();

        int xI = prun.vertex;
        int pI = prun.parent;

        bool xIsV = xI < n;
        bool isTwin = prun.type == DistH::PruningType::FalseTwin;


        // --- Lines 5 - 7 ---

        if (xIsV && isTwin)
        {
            int vId = xI;
            int uId = pI;

            int X = B(uId);
            B.setPsi(vId, X);
        }


        // --- Lines 8 - 10 ---

        else if (!xIsV && isTwin)
        {
            int eId = xI - n;
            int fId = pI - n; // E'

            int X = B[fId];
            B.setPhi(eId, X);
        }


        // --- Lines 11 - 16 ---

        else if (xIsV && !isTwin)
        {
            int vId = xI;
            int eId = pI - n;
            int X = B[eId];

            if (B.PhiSize(X) == 1 && B.inDegree(X) == 0)
            {
                B.setPsi(vId, X);
            }
            else
            {
                int Y = B.createNode();
                B.setPsi(vId, Y);
                B.setPhi(eId, Y);
                B.addEdge(Y, X);
            }
        }


        // --- Lines 17 - 22 ---

        else if (!xIsV && !isTwin)
        {
            int eId = xI - n;
            int vId = pI;
            int X = B(vId);

            if (B.PsiSize(X) == 1 && B.outDegree(X) == 0)
            {
                B.setPhi(eId, X);
            }
            else
            {
                int Y = B.createNode();
                B.setPsi(vId, Y);
                B.setPhi(eId, Y);
                B.addEdge(X, Y);
            }
        }
    }

    return B;
}

// Computes the edges of the subset graph of the given gamma-acyclic hypergraph.
// A pair (x, y) states that y is subset of x.
vector<intPair> GammaAcyclic::subsetGraph(const Hypergraph& h)
{
    // --- Algorithm 5 ---

    Bachman B = bachman(h);
    vector<intPair> result;

    for (int eId = 0; eId < h.getESize(); eId++)
    {
        vector<int> superSets = B.canReach(eId);

        for (const int& sId : superSets)
        {
            result.push_back(intPair(sId, eId));
        }
    }

    Sorting::radixSort(result);
    return result;
}
