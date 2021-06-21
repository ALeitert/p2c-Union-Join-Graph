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
    // Make sure vector is large enough.
    // Prevents an error in refine() when adding a new group can destroy references.
    groups.reserve(k);

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
// Returns the indices of the newly created groups.
vector<size_t> PartRefinement::refine(const vector<int>& idList)
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

    vector<size_t> newGroups;

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
        newGroups.push_back(newGrpIdx);

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

    return newGroups;
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
            l_GrpIdx = grpIdx;
            inLast.clear();
        }

        if (grpIdx == f_GrpIdx) inFirst.push_back(id);
        if (grpIdx == l_GrpIdx) inLast.push_back(id);
    }

    assert(f_GrpIdx != l_GrpIdx);
    assert(inFirst.size() > 0);
    assert(inLast.size() > 0);


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

        if (lGrpIdx == f_GrpIdx)
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

        if (fGrpIdx == l_GrpIdx)
        {
            fGrpIdx = newGrpIdx;
        }
        else
        {
            Group& prevGrp = groups[newGrp.prev];
            prevGrp.next = newGrpIdx;
        }

        // Update references for IDs in group.
        for (size_t i = newGrp.start; i <= newGrp.end; i++)
        {
            int vId = order[i];
            id2Grp[vId] = newGrpIdx;
        }
    }
}


// Refines the group (part) containing the given vertex x according to
// Rule 1 for factorising permutations.
// Is used to recognise cographs.
vector<size_t> PartRefinement::r1Refine(int xId, const vector<int>& xNeigh)
{
    // -- Rule 1 Refinement --
    // For a given part C and vertex x in C, split C into
    // [ co-N(x) \cap C, { x }, N(x) \cap C ].

    // -- Approach --
    // Determine C and N(x) \cap C.
    // Then, call Refine(N(x) \cap C) and Refine({ x }).


    size_t cIdx = id2Grp[xId];
    Group C = groups[cIdx];

    vector<int> neiList;

    for (const int& id : xNeigh)
    {
        size_t grpIdx = id2Grp[id];
        if (grpIdx == cIdx) neiList.push_back(id);
    }

    vector<size_t> grps;

    if (neiList.size() > 0)
    {
        // If x has neighbours in C, then refine() adds them into a new group
        // which follows C.

        refine(neiList);
        grps.push_back(C.next);
    }

    if (C.start < C.end)
    {
        // If x is not allone in C, then refine() adds it into a new group which
        // follows C.

        refine(vector<int> { xId });
        grps.push_back(cIdx);
    }

    // Add group of x to the front.
    grps.push_back(id2Grp[xId]);
    if (grps.size() > 1) swap(grps[0], grps.back());

    return grps;
}

// Refines the current groups (parts) which do not contain the given vertex
// y according to Rule 2 for factorising permutations.
// Returns the indices of the newly created groups.
// Is used to recognise cographs.
vector<size_t> PartRefinement::r2Refine(int yId, const vector<int>& yNeigh)
{
    // -- Rule 2 Refinement --
    // For a given vertex y, split all parts C not containing y into
    // [ co-N(y) \cap C, N(y) \cap C ].
    // Additionally, drop the group contining y if it is a singelton group.

    // -- Approach --
    // Determine set S of all neighbours of y which are not in the same
    // group (part). Then, call Refine(S).


    size_t yGrpIdx = id2Grp[yId];

    // No need to check neighbours if y is alone in its group.
    if (dropIfSingle(yGrpIdx))
    {
        return refine(yNeigh);
    }

    vector<int> neiList;

    for (const int& id : yNeigh)
    {
        size_t grpIdx = id2Grp[id];
        if (grpIdx != yGrpIdx) neiList.push_back(id);
    }

    return refine(neiList);
}

// Determines the first element in the group with the given index.
// Is used to recognise cographs.
int PartRefinement::firstInGroup(size_t grpIdx)
{
    assert(grpIdx < groups.size());

    const Group& grp = groups[grpIdx];
    return order[grp.start];
}

// Determines the nearest non-singleton groups to the left and right of the group containing the given element.
vector<size_t> PartRefinement::findLRNonSingles(int id)
{
    vector<size_t> result;

    size_t grpIdx = id2Grp[id];
    const Group& grp = groups[grpIdx];

    // Search on left.
    for (int lIdx = grp.prev; lIdx < groups.size(); lIdx = groups[lIdx].prev)
    {
        const Group& lGrp = groups[lIdx];
        if (lGrp.start >= lGrp.end) continue;

        // Goup has more than one entry.

        result.push_back(lIdx);
        break;
    }

    // Search on right.
    for (int rIdx = grp.next; rIdx < groups.size(); rIdx = groups[rIdx].next)
    {
        const Group& rGrp = groups[rIdx];
        if (rGrp.start >= rGrp.end) continue;

        // Goup has more than one entry.

        result.push_back(rIdx);
        break;
    }

    return result;
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

// Drops all groups at the end which only contain one element.
// Returns false if the data structure is empty afterwards.
bool PartRefinement::dropSingles()
{
    if (size() == 0) return false;

    while (size() > 0 && groups[lGrpIdx].start == groups[lGrpIdx].end)
    {
        lGrpIdx = groups[lGrpIdx].prev;
        grpCount--;

        if (grpCount > 0)
        {
            groups[lGrpIdx].next = -1;
        }
    }

    return size() > 0;
}

// Determines if the given ID is in a group with additional IDs.
bool PartRefinement::isDroppedOrSingle(int id) const
{
    assert(id >= 0 && id < id2Grp.size());

    size_t grpIdx = id2Grp[id];
    if (grpIdx == -1) return true;

    const Group& grp = groups[grpIdx];
    return grp.start == grp.end;
}


// Drops the group containing the given ID if it is a singleton group.
// Returns false if the given ID is not in a singleton group.
bool PartRefinement::dropIfSingle(int id)
{
    assert(id >= 0 && id < id2Grp.size());

    size_t grpIdx = id2Grp[id];
    if (grpIdx == -1) return true;

    const Group& grp = groups[grpIdx];
    if (grp.start < grp.end) return false;


    // ID is only element in group.
    // Remove group from list.


    size_t pIdx = grp.prev;
    size_t nIdx = grp.next;

    if (pIdx >= 0) groups[pIdx].next = nIdx;
    if (nIdx >= 0) groups[nIdx].prev = pIdx;

    if (fGrpIdx == grpIdx) fGrpIdx = nIdx;
    if (lGrpIdx == grpIdx) lGrpIdx = pIdx;

    grpCount--;
    id2Grp[id] = -1;

    return true;
}
