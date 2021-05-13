// Represents a weighted undirected graph.

#ifndef __Graph_H__
#define __Graph_H__


#include <vector>


#include "../helper.h"


using namespace std;


// Represents a weighted undirected graph.
class Graph
{
    // The graph is represented as adjacency list.
    // We assume that the graph is undirected.
    // Hence, each edge is represented twice.

public:

    // Default constructor.
    Graph();

    // Constructor.
    // Needs to be sorted by vertex-IDs, not contain duplicates, and from-ID > to-ID.
    Graph(const vector<intPair>& eList, const vector<int>& wList);

    // Destructor.
    ~Graph();


    // Returns the neighbours of the given vertex.
    const vector<int>& operator[](const int vId) const;

    // Returns the weights to neighbours of the given vertex.
    const vector<int>& operator()(const int vId) const;


    // The number of vertices.
    size_t size() const;


private:

    // Adjacency list.
    vector<int>* edges = nullptr;
    vector<int>* weights = nullptr;

    // Number of vertices.
    size_t vSize = 0;
};

#endif
