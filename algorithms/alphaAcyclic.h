// Contains algorithms for alpha-acyclic hypergraphs.

#ifndef __Algorithms_AlphaAcyclic_H__
#define __Algorithms_AlphaAcyclic_H__


#include "../dataStructures/hypergraph.h"


namespace AlphaAcyclic
{
    // Genrates an alpha-acyclic hypergraph with m edges and total size N.
    Hypergraph genrate(size_t m, size_t N);
}

#endif
