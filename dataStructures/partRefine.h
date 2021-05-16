// Implements a Partition-Refinement data structure.

#ifndef __PartRefine_H__
#define __PartRefine_H__


#include <cstdlib>
#include <vector>

using namespace std;


class PartRefinement
{
    // Represents a group within the partition refinement.
    struct Group
    {
        // The index of the first element in the group (inclusive).
        size_t start = 0;

        // The index of the last element in the group (inclusive).
        size_t end = 0;

        // The index of the next group.
        size_t nextId = 0;

        // A temporary counter how many elements will be removed from the group.
        size_t count = 0;
    };


public:

    // Default constructor.
    // Creates an empty data structure.
    PartRefinement();

    // Constructor.
    // Creates a partition refinement of size k.
    PartRefinement(size_t k);


private:

    // All groups (not in order).
    vector<Group> groups;

    // All IDs in their current order.
    vector<int> order;

    // States for each ID in which group it is.
    vector<size_t> id2Grp;

    // States for each ID where it is in the order.
    vector<size_t> id2Ord;
};

#endif
