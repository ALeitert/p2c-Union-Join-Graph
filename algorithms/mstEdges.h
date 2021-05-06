// Provides algorithms to compute all edges of a graph which are part of a maximum spanning tree.

#ifndef __Algorithms_MstEdges_H__
#define __Algorithms_MstEdges_H__


#include <vector>

#include "../dataStructures/graph.h"
#include "../helper.h"


namespace MstEdges
{
    // A reference to a function that computes the subset graph of a given hypergraph.
    typedef vector<sizePair> (&mstAlgo)(const Graph&);


    // Determines all edges which are part of a MaxST by checking each edge individually.
    vector<sizePair> checkAllEdges(const Graph& g);

    // Determines all edges which are part of a MaxST based on Kruskal's algorithm.
    vector<sizePair> kruskal(const Graph& g);
}

#endif
