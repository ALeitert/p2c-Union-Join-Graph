#include <cassert>
#include <stdexcept>

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

// Refines the the first and last group that contain any of the given IDs.
// Refinement happens towards each other instead of towards the end.
void PartRefinement::flRefine(const vector<int>& idList)
{
    // --- Find first and last group. ---

    size_t lstIdx = 0;
    int id0 = -1;

    for (; lstIdx < idList.size(); lstIdx++)
    {
        int id = idList[lstIdx];
        size_t grpIdx = id2Grp[id];

        if (grpIdx == -1) continue;

        id0 = id;
        break;
    }
    assert(id0 != -1);

    size_t f_GrpIdx = id2Grp[id0];
    size_t l_GrpIdx = id2Grp[id0];

    // IDs that are in the first or last group, respectively.
    vector<int> inFirst;
    vector<int> inLast;

    for (; lstIdx < idList.size(); lstIdx++)
    {
        int id = idList[lstIdx];
        size_t grpIdx = id2Grp[id];
        if (grpIdx == -1) continue;

        Group& f_Grp = groups[f_GrpIdx];
        Group& l_Grp = groups[l_GrpIdx];

        Group& grp = groups[grpIdx];

        if (grp.start < f_Grp.start)
        {
            f_GrpIdx = grpIdx;
            inFirst.clear();
        }

        if (grp.end > l_Grp.end)
        {
            f_GrpIdx = grpIdx;
            inLast.clear();
        }

        if (grpIdx == f_GrpIdx) inFirst.push_back(id);
        if (grpIdx == l_GrpIdx) inLast.push_back(id);
    }

    assert(f_GrpIdx != l_GrpIdx);


    // --- Flag IDs in first group and move to the ond of the group. ---

    Group& f_Grp = groups[f_GrpIdx];

    for (const int& id : inFirst)
    {
        // Move to end of group.
        int endId = order[f_Grp.end - f_Grp.count];

        size_t& ordIdx = id2Ord[id];
        size_t& endIdx = id2Ord[endId];

        swap(order[ordIdx], order[endIdx]);
        swap(ordIdx, endIdx);

        f_Grp.count++;
    }


    // --- Flag IDs in last group and move to the beginning of the group. ---

    Group& l_Grp = groups[l_GrpIdx];

    for (const int& id : inLast)
    {
        // Move to beginning of group.
        int startId = order[l_Grp.start + l_Grp.count];

        size_t& orderIdx = id2Ord[id];
        size_t& startIdx = id2Ord[startId];

        swap(order[orderIdx], order[startIdx]);
        swap(orderIdx, startIdx);

        l_Grp.count++;
    }


    // --- Split first group. ---

    // Full group marked?
    if (f_Grp.end - f_Grp.start + 1 == f_Grp.count)
    {
        // No need to split.
        f_Grp.count = 0;
    }
    else
    {
        size_t newGrpIdx = groups.size();
        groups.push_back(Group());
        Group& newGrp = groups.back();
        grpCount++;

        newGrp.end = f_Grp.end;
        newGrp.start = f_Grp.end - f_Grp.count + 1;
        newGrp.prev = f_GrpIdx;
        newGrp.next = f_Grp.next;

        f_Grp.end -= f_Grp.count;
        f_Grp.next = newGrpIdx;
        f_Grp.count = 0;

        // Note that, since f_GrpIdx != l_GrpIdx, f_Grp cannot be the last group
        // in the data structure overall, i.e., there is a next group.
        Group& nextGrp = groups[newGrp.next];
        nextGrp.prev = newGrpIdx;

        // Update references for IDs in group.
        for (size_t i = newGrp.start; i <= newGrp.end; i++)
        {
            int vId = order[i];
            id2Grp[vId] = newGrpIdx;
        }
    }


    // --- Split last group. ---

    // Full group marked?
    if (l_Grp.end - l_Grp.start + 1 == l_Grp.count)
    {
        // No need to split.
        l_Grp.count = 0;
    }
    else
    {
        size_t newGrpIdx = groups.size();
        groups.push_back(Group());
        Group& newGrp = groups.back();
        grpCount++;

        newGrp.start = l_Grp.start;
        newGrp.end = l_Grp.start + l_Grp.count - 1;
        newGrp.prev = l_Grp.prev;
        newGrp.next = l_GrpIdx;

        l_Grp.start += l_Grp.count;
        l_Grp.prev = newGrpIdx;
        l_Grp.count = 0;

        Group& prevGrp = groups[newGrp.prev];
        prevGrp.next = newGrpIdx;

        // Update references for IDs in group.
        for (size_t i = newGrp.start; i <= newGrp.end; i++)
        {
            int vId = order[i];
            id2Grp[vId] = newGrpIdx;
        }
    }
}

// The current number of non-empty groups.
size_t PartRefinement::size() const
{
    return grpCount;
}

// Returns the current order.
const vector<int>& PartRefinement::getOrder() const
{
    return order;
}

// Returns the last ID in the last group.
int PartRefinement::last() const
{
    if (size() == 0) throw logic_error("Data structure is empty.");

    const Group& grp = groups[lGrpIdx];
    return order[grp.end];
}

// "Removes" the last ID in the order from its group.
// If the group becomes empty, it will be removed.
void PartRefinement::dropLast()
{
    if (size() == 0) throw logic_error("Data structure is empty.");

    // Last group.
    Group& grp = groups[lGrpIdx];

    // Update ID.
    int lastId = order[grp.end];
    id2Grp[lastId] = -1;


    if (grp.start == grp.end)
    {
        // Single element. Remove group.

        lGrpIdx = grp.prev;
        grpCount--;

        if (grpCount > 0)
        {
            groups[lGrpIdx].next = -1;
        }
    }
    else
    {
        // Multiple elements in group. Just shift the end.
        grp.end--;
    }
}
