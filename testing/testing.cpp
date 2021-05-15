#include <iostream>
#include <chrono>

#include "../algorithms/alphaAcyclic.h"
#include "subsetTest.h"
#include "testing.h"

using namespace std;
using namespace std::chrono;


// See https://stackoverflow.com/a/11376759.
#ifndef __Log_Ceil__
#define LogC(X) ((unsigned) ((sizeof(unsigned long long) << 3) - __builtin_clzll((X) - 1)))
#endif


// Tests a given function that computes the subset graph of a given hypergraph.
void Testing::testGeneralSSG(SubsetGraph::ssgAlgo ssg, int seed, int tests, int maxSize)
{
    srand(seed);

    cout << "Testing Subset Graph Implementation for General Hypergraphs." << endl
         << tests << " test cases with max. size " << maxSize << "." << endl;

    bool allPassed = true;

    for (int i = 1, perc = -1; i <= tests; i++)
    {
        // --- Run test. ---

        SubsetTest sst;
        vector<intPair> answer;

        int size = min(i, rand() % maxSize) + 5;
        const Hypergraph& hg = sst.build(size);

        try
        {
            // Run algorithm.
            answer = ssg(hg);
        }
        catch (const exception& e)
        {
            cout << "Test " << i << " failed with exception." << endl;
            cerr << e.what() << endl;

            allPassed = false;
            break;
        }

        bool correct = sst.verify(answer);


        // Cancel if test case fails.
        if (!correct)
        {
            cout << "Test " << i << " failed." << endl;
            cout << "  answer: "; print(answer);
            cout << "solution: "; print(sst.getSolution());

            hg.print(cout);

            allPassed = false;
            break;
        }


        // --- Print progress. ---

        int progress = (i * 100) / tests;

        if (progress > perc)
        {
            perc = progress;

            // Based on https://stackoverflow.com/a/21870633.
            cout << perc << " %\r" << flush;
        }

        if (perc == 100) cout << endl;
    }

    if (allPassed)
    {
        cout << "All tests passed." << endl;
    }
}

// Tests a given function that computes the subset graph of a given hypergraph.
// Returns the time needed in milliseconds.
uint64_t Testing::unionJoinGraph(ujgAlgo algo, string name, unsigned int seed, size_t tests, size_t maxSize)
{
    cout << "\nTesting Union Join Graph Implementation: " << name << endl
         << tests << " test cases with max. size " << maxSize << "." << endl;


    srand(seed);
    auto start = high_resolution_clock::now();


    for (size_t tNo = 1, perc = -1; tNo < tests; tNo++)
    {
        // --- Determine size and create hypergraph. ---

        size_t sz = rand() % maxSize + 5;
        size_t N = sz + rand() % (sz * LogC(sz)) + 1;

        Hypergraph aaHg = AlphaAcyclic::genrate(sz, N);


        // --- Run tests. ---

        try
        {
            Graph ujg = algo(aaHg);
        }
        catch (const exception& e)
        {
            cout << "Test " << tNo << " failed with exception." << endl;
            cerr << e.what() << endl;
            break;
        }


        // --- Print progress. ---

        int progress = (tNo * 100) / tests;

        if (progress != perc)
        {
            perc = progress;
            cout << perc << " %\r" << flush;
        }
    }

    auto end = high_resolution_clock::now();
    return duration_cast<milliseconds>(end - start).count();
}

// Tests if two given algorithms return the same result.
bool Testing::unionJoinGraph(ujgAlgo algo1, ujgAlgo algo2, unsigned int seed, size_t tests, size_t maxSize)
{
    cout << "\nTesting Result of Union Join Graph Implementations." << endl
         << tests << " test cases with max. size " << maxSize << "." << endl;


    srand(seed);
    bool allPassed = true;


    for (size_t tNo = 1, perc = -1; tNo < tests; tNo++)
    {
        // --- Determine size and create hypergraph. ---

        size_t sz = rand() % maxSize + 5;
        size_t N = sz + rand() % (sz * LogC(sz)) + 1;

        Hypergraph aaHg = AlphaAcyclic::genrate(sz, N);


        // --- Run tests. ---

        bool equal = false;

        try
        {
            equal = UnionJoinTest::compareAlgorithms(aaHg, algo1, algo2);
        }
        catch (const exception& e)
        {
            cout << "Test " << tNo << " failed with exception." << endl;
            cerr << e.what() << endl;
            allPassed = false;
            break;
        }


        if (!equal)
        {
            cout << "Test " << tNo << " failed." << endl;
            allPassed = false;
            break;
        }


        // --- Print progress. ---

        int progress = (tNo * 100) / tests;

        if (progress != perc)
        {
            perc = progress;
            cout << perc << " %\r" << flush;
        }
    }

    if (allPassed)
    {
        cout << "All tests passed." << endl;
    }

    return allPassed;
}
