// Represents a weighted undirected graph.

#ifndef __Graph_H__
#define __Graph_H__


#include <vector>

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


    // Returns the neighbours of the given vertex.
    const vector<size_t>& operator[](const size_t vId) const;

    // Returns the weights to neighbours of the given vertex.
    const vector<int>& operator()(const size_t vId) const;


    // The number of vertices.
    size_t size() const;


private:

    // Adjacency list.
    vector<size_t>* edges = nullptr;
    vector<int>* weights = nullptr;

    // Number of vertices.
    size_t vSize = 0;
};

#endif
