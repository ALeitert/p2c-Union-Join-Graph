#include <iostream>

#include "subsetTest.h"
#include "testing.h"

using namespace std;


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
