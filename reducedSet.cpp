#include "reducedSet.h"

// Default constructor.
// Creates an empty set.
ReducedSet::ReducedSet() :
    n(0),
    R(new wordIndex[0])
{
    // Do nothing.
}

// Constructor.
// Creates a set from a list of elements.
// Assumes that the given list is sorted.
ReducedSet::ReducedSet(const vector<int>& list)
{
    // https://stackoverflow.com/a/11376759
    #define LOG2(X) ((unsigned) ((sizeof(unsigned long long) << 3) - __builtin_clzll((X)) - 1))

    // Number of bits in a word.
    const unsigned int wordSize = sizeof(word) << 3 /* times 8 */;

    // Allows bit operations istead of division and modulo.
    const unsigned int wordDiv = LOG2(wordSize);
    const unsigned int wordMod = wordSize - 1;

    #undef LOG2


    vector<wordIndex> rBuilder;

    for (int i = 0, lastIdx = -1; i < list.size(); i++)
    {
        int id = list[i];

        int wordIdx = id >> wordDiv;
        int wordBit = id & wordMod;

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
    if (R != nullptr) delete R;
}
