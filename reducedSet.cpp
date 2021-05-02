#include <stdexcept>

#include "reducedSet.h"


// Copy constructor.
ReducedSet::ReducedSet(const ReducedSet& set) :
    n(set.n),
    R(new wordIndex[set.n])
{
    copy(set.R, set.R + set.n, R);
}

// Move constructor.
ReducedSet::ReducedSet(ReducedSet&& set) :
    n(set.n),
    R(set.R)
{
    set.n = -1;
    set.R = nullptr;
}

// Constructor.
// Creates a set from a list of elements.
// Assumes that the given list is sorted.
ReducedSet::ReducedSet(const vector<int>& list)
{
    vector<wordIndex> rBuilder;

    for (int i = 0, lastIdx = -1; i < list.size(); i++)
    {
        int id = list[i];

        int wordIdx = id >> WordDiv;
        int wordBit = id & WordMod;

        if (wordIdx > lastIdx)
        {
            lastIdx = wordIdx;
            rBuilder.push_back(wordIndex(wordIdx, 0));
        }

        word& lastWord = rBuilder.back().second;
        lastWord |= 1 << wordBit;
    }


    // Store results.
    n = rBuilder.size();
    R = new wordIndex[n];
    copy(rBuilder.begin(), rBuilder.end(), R);
}


// Destructor.
ReducedSet::~ReducedSet()
{
    if (R != nullptr) delete[] R;
}


// Move assignment.
ReducedSet& ReducedSet::operator=(ReducedSet&& set)
{
    if (R != nullptr) delete[] R;

    n = set.n;
    R = set.R;

    set.n = -1;
    set.R = nullptr;

    return *this;
}


// Computes the intersection of two sets and returns the result as a new set.
ReducedSet ReducedSet::operator&(const ReducedSet& rhs) const
{
    const ReducedSet& lhs = (*this);

    vector<wordIndex> rBuilder;

    // Compute intersection.
    for (int l = 0, r = 0; l < lhs.n && r < rhs.n; )
    {
        int lIdx = lhs.R[l].first;
        int rIdx = rhs.R[r].first;

        if (lIdx == rIdx)
        {
            word lWrd = lhs.R[l].second;
            word rWrd = rhs.R[r].second;

            rBuilder.push_back(wordIndex(lIdx, lWrd & rWrd));
        }

        if (lIdx <= rIdx) l++;
        if (lIdx >= rIdx) r++;
    }



    // Store results.
    int newN = rBuilder.size();
    wordIndex* newR = new wordIndex[newN];
    copy(rBuilder.begin(), rBuilder.end(), newR);

    return ReducedSet(newN, newR);
}

// Changes the current set to be the intersection of the current and given set.
void ReducedSet::operator&=(const ReducedSet& rhs)
{
    ReducedSet& lhs = (*this);

    int newN = 0;

    // Compute intersection.
    for (int l = 0, r = 0; l < lhs.n && r < rhs.n; )
    {
        int lIdx = lhs.R[l].first;
        int rIdx = rhs.R[r].first;

        if (lIdx == rIdx)
        {
            word lWrd = lhs.R[l].second;
            word rWrd = rhs.R[r].second;

            lhs.R[newN] = wordIndex(lIdx, lWrd & rWrd);
            newN++;
        }

        if (lIdx <= rIdx) l++;
        if (lIdx >= rIdx) r++;
    }

    // Update size.
    lhs.n = newN;
}

// Creates an iterator pointing to the first element of the set.
ReducedSet::Iterator ReducedSet::begin() const
{
    return Iterator::begin(*this);
}

// Creates an iterator pointing to the end of the set.
const ReducedSet::Iterator ReducedSet::end() const
{
    return Iterator::end(*this);
}



// --------------------------------
// --- --- --- Iterator --- --- ---


// Copy constructor.
ReducedSet::Iterator::Iterator(const Iterator& it) :
    ptr(it.ptr),
    length(it.length),
    bitIdx(it.bitIdx)
{
    // Nothing to do.
}

// Copy assignment.
ReducedSet::Iterator& ReducedSet::Iterator::operator=(const Iterator& rhs)
{
    ptr = rhs.ptr;
    length = rhs.length;
    bitIdx = rhs.bitIdx;
}


// Prefix increment operator.
ReducedSet::Iterator& ReducedSet::Iterator::operator++()
{
    findNext();
    return *this;
}

// Postfix increment operator.
ReducedSet::Iterator ReducedSet::Iterator::operator++(int)
{
    Iterator copy(*this);
    findNext();
    return copy;
}


// Equality comparison.
bool ReducedSet::Iterator::operator==(const Iterator& rhs) const
{
    const Iterator& lhs = *this;

    return
        lhs.ptr == rhs.ptr &&
        lhs.length == rhs.length &&
        lhs.bitIdx == rhs.bitIdx;
}

// Inequality comparison.
bool ReducedSet::Iterator::operator!=(const Iterator& rhs) const
{
    return !operator==(rhs);
}


// Dereference operator.
int ReducedSet::Iterator::operator*() const
{
    int wIdx = ptr[0].first;
    int mod = 1 << bitIdx;

    return (wIdx * WordDiv) | mod;
}


// Creates an iterator that points to the beginning of a given set.
// Is equal to end if set is empty or invalid.
ReducedSet::Iterator ReducedSet::Iterator::begin(const ReducedSet& set)
{
    Iterator it;
    if (set.R == nullptr) return it;

    it.ptr = set.R;
    it.length = set.n;

    if (it.length >= 0)
    {
        word w = it.ptr[0].second;

        if (w == 0)
        {
            throw logic_error("Invalid ReducedSet: contains all-zero entries.");
        }

        // Find first non-zero bit.
        // Number of trailing 0s (i.e., starting from LSB).
        it.bitIdx = __builtin_ctzl(w);
    }

    return it;
}

// Creates an iterator that points to the end of a given set.
const ReducedSet::Iterator ReducedSet::Iterator::end(const ReducedSet& set)
{
    Iterator it;
    if (set.R == nullptr) return it;

    it.ptr = set.R + set.n;
    it.length = 0;

    return it;
}


// Helper function that finds the next entry in the set.
void ReducedSet::Iterator::findNext()
{
    if (ptr == nullptr || length <= 0)
    {
        throw logic_error("Iterator already at end of ReducedSet.");
    }

    // Move at least one bit.
    bitIdx++;
    word w = ptr[0].second >> bitIdx;

    if (w > 0)
    {
        // Still bits in the current word.
        // Shift to it, then done.

        // Number of trailing 0s (i.e., starting from LSB).
        bitIdx += __builtin_ctzl(w);
        return;
    }


    // No more bits in current word.
    // Go to next word.

    ptr++;
    length--;
    bitIdx = 0;

    // Reached end of sedt. Done.
    if (length <= 0) return;


    // Find first non-zero bit.
    w = ptr[0].second;

    if (w == 0)
    {
        throw logic_error("Invalid ReducedSet: contains an all-zero entry.");
    }

    // Determines number of trailing 0s (i.e., starting from LSB).
    bitIdx = __builtin_ctzl(w);
}
