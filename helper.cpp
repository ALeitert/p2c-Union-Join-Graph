#include <algorithm>
#include <iomanip>
#include <iostream>

#include "algorithms/sorting.h"
#include "helper.h"


size_t intPairHash::operator() (intPair const &pair) const
{
    size_t h1 = hash<int>()(pair.first);
    size_t h2 = hash<int>()(pair.second);

    h1 = ((h1 << shift) & high) | ((h1 >> shift) & low);

    return h1 ^ h2;
}


// Prints a list of integer pairs to the terminal.
void print(const vector<intPair>& pairs)
{
    for (int i = 0; i < pairs.size(); i++)
    {
        if (i > 0) cout << " | ";
        cout << pairs[i].first << " " << pairs[i].second;
    }
    cout << endl;
}

// Prints the given time (in milliseconds) into the given stream using appropriate unit.
void printTime(int64_t time, ostream& out)
{
    const int64_t milSec = 1;
    const int64_t decSec = 100 * milSec;
    const int64_t second = 10 * decSec;
    const int64_t minute = 60 * second;
    const int64_t hour = 60 * minute;
    const int64_t day = 24 * hour;

    if (time < second)
    {
        out << setw(3) << time << " ms";
    }
    else if (time <= minute)
    {
        int64_t dSecs = (time + decSec - 1) / decSec;
        int64_t secs = dSecs / 10;
        dSecs = dSecs % 10;

        out << setw(2) << secs << "." << dSecs << " s";
    }
    else if (time <= hour)
    {
        int64_t secs = (time + second - 1) / second;
        int64_t mins = secs / 60;
        secs = secs % 60;

        out << setw(2) << mins << " min " << setw(2) << secs << " s";
    }
    else if (time <= day)
    {
        int64_t mins = (time + minute - 1) / minute;
        int64_t hours = mins / 60;
        mins = mins % 60;

        out << setw(2) << hours << " h " << setw(2) << " min";
    }
    else // time > day
    {
        int64_t hours = (time + hour - 1) / hour;
        int64_t days = hours / 24;
        hours = hours % 24;

        out << days << " d " << setw(2) << hour << " h";
    }
    out << flush;
}


void makePermutation(int* arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        arr[i] = i;
    }

    // No idea why std::begin()/end() do not work.
    random_shuffle(arr, arr + size);
}
