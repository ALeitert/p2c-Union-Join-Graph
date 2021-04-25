#ifndef __Hypergraph_H__
#define __Hypergraph_H__

#include <vector>

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

    // Destructor.
	~Hypergraph();


    // The number n of vertices.
    int getVSize() const;

    // The number m of hyperedges.
    int getESize() const;

    // The combined size of all hyperedges.
    int getTotalSize() const;

    // Returns the hyperedge with index i.
    const vector<int>& operator[](const int i) const;

    // Returns the hyperedges containing the vertex with index j.
    const vector<int>& operator()(const int j) const;

    // Constructor.
    // Initialises a new hypergraph with n vertices and m hyperedges.
    // Does not create any edges in the bipartite representation.
    Hypergraph(const int n, const int m);


private:

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
