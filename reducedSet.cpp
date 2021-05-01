#include "reducedSet.h"

// Default constructor.
// Creates an empty set.
ReducedSet::ReducedSet() :
    n(0),
    R(new wordIndex[0])
{
    // Do nothing.
}

// Destructor.
ReducedSet::~ReducedSet()
{
    if (R != nullptr) delete R;
}
