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
}

#endif
