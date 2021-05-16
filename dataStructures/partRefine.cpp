#include "partRefine.h"


// Default constructor.
// Creates an empty data structure.
PartRefinement::PartRefinement() { /* Do nothing. */ }

// Constructor.
// Creates a partition refinement of size k.
PartRefinement::PartRefinement(size_t k)
{
    // One group containing all.
    groups.push_back(Group());
    groups[0].end = k - 1;
    groups[0].prev = -1;
    groups[0].next = -1;
    grpCount = 1;

    // All in group 0.
    id2Grp.resize(k, 0);

    // Create order and store indices.
    order.resize(k);
    id2Ord.resize(k);

    for (int i = 0; i < k; i++)
    {
        order[i] = i;
        id2Ord[i] = i;
    }
}
