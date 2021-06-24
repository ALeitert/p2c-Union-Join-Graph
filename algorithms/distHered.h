// Contains algorithms for distance-hereditarys graphs.

#ifndef __Algorithms_DistH_H__
#define __Algorithms_DistH_H__


#include "../dataStructures/graph.h"


namespace DistH
{
    // Represents the different types of pruning operations.
    enum class PruningType
    {
        Pendant,
        FalseTwin,
        TrueTwin
    };

    // Represents a single pruning operation.
    struct Pruning
    {
        int vertex;
        PruningType type;
        int parent;

        // Default constructor.
        Pruning() = default;

        // Constructor.
        Pruning(int vId, PruningType pType, int pId) :
            vertex(vId),
            type(pType),
            parent(pId)
        {
            /* Nothing to do. */
        }
    };


    // Computes a pruning sequence for a given cograph.
    // Returns an empty list if the given graph is not a cograph.
    vector<Pruning> pruneCograph(const Graph& g);

    // Computes a pruning sequence for a given cograph.
    // Returns an empty list if the given graph is not a cograph.
    vector<Pruning> pruneCograph_noTree(const Graph& g);
}

#endif
