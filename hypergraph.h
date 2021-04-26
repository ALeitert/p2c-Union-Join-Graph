#ifndef __Hypergraph_H__
#define __Hypergraph_H__

#include <utility>
#include <vector>

using namespace std;


typedef pair<int, int> evPair;

// Based on https://www.techiedelight.com/use-pair-key-std-unordered_set-cpp/
// Allows to use evPair with unordered_set class.
class evPairHash
{
    static const size_t shift = sizeof(size_t) / 2;
    static const size_t low = (1 << shift) - 1;
    static const size_t high = low << shift;

public:
    size_t operator() (evPair const &pair) const
    {
        size_t h1 = hash<int>()(pair.first);
        size_t h2 = hash<int>()(pair.second);

        h1 = ((h1 << shift) & high) | ((h1 >> shift) & low);

        return h1 ^ h2;
    }
};



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
    Hypergraph(const vector<evPair>& eList);

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
    Hypergraph& operator= (Hypergraph&&);


    // Returns the hyperedge with index i.
    const vector<int>& operator[](const int i) const;

    // Returns the hyperedges containing the vertex with index j.
    const vector<int>& operator()(const int j) const;


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
