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
    // Returns the indices of the newly created groups.
    vector<size_t> refine(const vector<int>& idList);

    // Refines the the first and last group that contain any of the given IDs.
    // Refinement happens towards each other instead of towards the end.
    void flRefine(const vector<int>& idList);


    // Refines the group (part) containing the given vertex x according to
    // Rule 1 for factorising permutations.
    // Is used to recognise cographs.
    vector<size_t> r1Refine(int xId, const vector<int>& xNeigh);

    // Refines the current groups (parts) which do not contain the given vertex
    // y according to Rule 2 for factorising permutations.
    // Returns the indices of the newly created groups.
    // Is used to recognise cographs.
    vector<size_t> r2Refine(int yId, const vector<int>& yNeigh);


    // The current number of non-empty groups.
    size_t size() const;

    // Returns the current order.
    const vector<int>& getOrder() const;


    // Returns the last ID in the last group.
    int last() const;

    // "Removes" the last ID in the order from its group.
    // If the group becomes empty, it will be removed.
    void dropLast();

    // Drops all groups at the end which only contain one element.
    // Returns false if the data structure is empty afterwards.
    bool dropSingles();

    // Determines if the given ID is either dropped or allone in its group.
    bool isDroppedOrSingle(int id) const;


    // Drops the group containing the given ID if it is a singleton group.
    // Returns false if the given ID is not in a singleton group.
    bool dropIfSingle(int id);


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
