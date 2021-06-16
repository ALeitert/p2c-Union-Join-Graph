// Contains algorithms for gamma-acyclic hypergraphs.

#ifndef __Algorithms_GammaAcyclic_H__
#define __Algorithms_GammaAcyclic_H__


#include "../dataStructures/hypergraph.h"


namespace GammaAcyclic
{
    // Genrates a gamma-acyclic hypergraph with n vertices and m hyperedges.
    Hypergraph genrate(size_t m, size_t n);
}

#endif
