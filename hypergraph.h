#ifndef __Hypergraph_H__
#define __Hypergraph_H__

#include <ostream>
#include <utility>
#include <vector>

#include "helper.h"

using namespace std;


/**
 *  Represents a hypergraph (or family of sets) as bipartite graph using an adjacency list.
 */
class Hypergraph
{
public:

    // Default constructor.
    // Creates an empty hypergraph.
    Hypergraph();

    // Constructor.
    // Creates a hypergraph based on an edge list.
    // Edges go from a hyperede to a vertex.
    Hypergraph(const vector<intPair>& eList);

    // Move constructor.
    Hypergraph(Hypergraph&&);

    // Destructor.
	~Hypergraph();


    // The number n of vertices.
    int getVSize() const;

    // The number m of hyperedges.
    int getESize() const;

    // The combined size of all hyperedges.
    int getTotalSize() const;


    // Move assignment.
    Hypergraph& operator=(Hypergraph&&);


    // Returns the hyperedge with index i.
    const vector<int>& operator[](const int i) const;

    // Returns the hyperedges containing the vertex with index j.
    const vector<int>& operator()(const int j) const;


    // Prints the hypergraph as family of sets.
    void print(ostream& out) const;


private:

    // Constructor.
    // Initialises a new hypergraph with n vertices and m hyperedges.
    // Does not create any edges in the bipartite representation.
    Hypergraph(const int n, const int m);

    // Helper function for constructors.
    // Initialises a new hypergraph with n vertices and m hyperedges,
    // and sets the total size to N.
    // Does not create any edges in the bipartite representation.
    void initialize(const int n, const int m, const int N);

    // Helper function for destructor and similar operations.
    // Frees occupied memory.
    void destruct();


    // The number n of vertices.
    int vSize = -1;

    // The number m of hyperedges.
    int eSize = -1;

    // The combined size of all hyperedges.
    int tSize = -1;


    // Set of vertices and the hyperedges containing them.
    vector<int>* vertices = nullptr;

    // Set of hyperedges and the vertices they contain.
    vector<int>* hyperedges = nullptr;
};

#endif
