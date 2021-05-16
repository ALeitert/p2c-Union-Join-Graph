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

        // The indices previous and next group.
        size_t prev = 0;
        size_t next = 0;

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


    // Refines the current groups based on the given list of IDs.
    void refine(const vector<int>& idList);


private:

    // All groups (not in order).
    vector<Group> groups;

    // The indices of first and last group.
    size_t fGrpIdx = 0;
    size_t lGrpIdx = 0;

    // The total number of non-empty groups.
    size_t grpCount = 0;

    // All IDs in their current order.
    vector<int> order;

    // States for each ID in which group it is.
    vector<size_t> id2Grp;

    // States for each ID where it is in the order.
    vector<size_t> id2Ord;
};

#endif
