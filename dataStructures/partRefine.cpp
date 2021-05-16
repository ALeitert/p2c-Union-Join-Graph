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

// Refines the current groups based on the given list of IDs.
void PartRefinement::refine(const vector<int>& idList)
{
    vector<size_t> modifiedGrps;

    // --- Flag IDs and move them to the end of their respective groups. ---

    for (const int& id : idList)
    {
        // Find group and swap with last element of that group.

        size_t grpIdx = id2Grp[id];
        if (grpIdx == -1) continue;

        Group& grp = groups[grpIdx];
        int endId = order[grp.end - grp.count];

        size_t& ordIdx = id2Ord[id];
        size_t& endIdx = id2Ord[endId];

        swap(order[ordIdx], order[endIdx]);
        swap(ordIdx, endIdx);

        // If needed, add to list of modified groups (needed for splitting).
        if (grp.count == 0) modifiedGrps.push_back(grpIdx);
        grp.count++;
    }


    // --- Split modified groups. ---

    for (const size_t& grpIdx : modifiedGrps)
    {
        Group& grp = groups[grpIdx];

        // Full group marked?
        if (grp.end - grp.start + 1 == grp.count)
        {
            // No need to split.
            grp.count = 0;
            continue;
        }

        size_t newGrpIdx = groups.size();
        groups.push_back(Group());
        Group& newGrp = groups.back();
        grpCount++;

        newGrp.end = grp.end;
        newGrp.start = grp.end - grp.count + 1;
        newGrp.prev = grpIdx;
        newGrp.next = grp.next;

        grp.end -= grp.count;
        grp.next = newGrpIdx;
        grp.count = 0;

        if (lGrpIdx == grpIdx)
        {
            lGrpIdx = newGrpIdx;
        }
        else
        {
            Group& nextGrp = groups[newGrp.next];
            nextGrp.prev = newGrpIdx;
        }

        // Update references for IDs in group.
        for (size_t i = newGrp.start; i <= newGrp.end; i++)
        {
            int vId = order[i];
            id2Grp[vId] = newGrpIdx;
        }
    }
}
