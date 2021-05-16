// Implements a Partition-Refinement data structure.

#ifndef __PartRefine_H__
#define __PartRefine_H__


#include <cstdlib>


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


private:


};

#endif
