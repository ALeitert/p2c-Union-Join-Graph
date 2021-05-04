# Union Join and Subset Graph of Acyclic Hypergraphs

Goal of this project is to implement the algorithms in my paper [1].
It investigates the two problems of computing the union join graph as well as computing the subset graph for acyclic hypergraphs and their subclasses.

The *union join graph G* of a given acyclic hypergraph *H* is the union of all its join trees.
That is, each vertex of *G* represents a hyperedge of *H* and two vertices of *G* are adjacent if there exits a join tree *T* for *H* such that the corresponding hyperedges are adjacent in *T*.

The *subset graph* of a hypergraph *H* is a directed graph *G* where each vertex represents a hyperedge of *H* and there is a directed edge from a vertex *u* to a vertex *v* if the hyperedge corresponding to *u* is a subset of the hyperedge corresponding to *v*.


## Computing the Subset Graph

A core part of the algorithms in [1] is the computation of a subset graph.
Indeed, most algorithms to compute union join graphs use a subset graph algorithm.
The paper presents such algorithms for β-acyclic, γ-acyclic, and interval hypergraphs.
For α-acylic hypergraphs, however, we need a subset graph algorithm for general graphs.
We also need such an algorithm to test our implementation.

Pritchard [2] present algorithms in their paper which compute the subset graph for an arbitrary hypergraph.
They start with a simple, high-level algorithm and then modify it two times with the goal of improving the runtime.
Pritchard shows that the runtime with all these modifications is in *O(N^2 log log N / log^2 N)*.


### Runtime Tests

I implemented four algorithms to compute subset graphs:

 1. A naive algorithm that checks each pair of hyperedges and determines if one is subset of the other.

 2. Pritchard's simple approach (see section 2 in [2]).
    It is a high-level approach without much optimisation that is foundation for the algorithms below.
    A core part of their algorithm is the computation of intersections of sets.

 3. Pritchard's algorithm a more compact representation for sets (see section 3 in [2]).
    We call these *reduced sets*.
    These sets allow for a faster computation of intersections.

 4. Pritchard's refined algorithm with sorting of vertices and hyperedges plus a "history" of already computed intersections (see section 4 in [2]).
    The idea for this approach is to keep track of previously computed intersection to avoid computing them again.
    To maximise the benefit of that, vertices and hyperedges are sorted in the begining.

When testing the algorithms, I generated 5000 random test cases with maximum size of 2000 hyperedges per hypergraph.
Note that each algorithm recieved the same 5000 hypergraphs (ensured by resetting the random number seed).

The times in the table below state the runtime for these tests.
The measured times include the generation of all test gases, the computation of the subset graph, and the verification of results.
In addition to the actual algorithms, I did run a reference-test which "solved" the problems by simply copying the generated solution.
That way, we get an estimation for how much of the measured time belongs to generation and verification.


| Algorithm           |   Time | Difference |
| :-----------------: | -----: | ---------: |
| Reference           |  6.8 s |            |
| Naive               | 53.6 s |     46.8 s |
| Pritchard Simple    | 15.0 s |      8.2 s |
| + Reduced Sets      |  9.5 s |      2.7 s |
| + Sorting + History | 10.8 s |      4.0 s |


One can clearly see that Pritchard's approach (even in its simplest form) is much faster than a naive implementation.
Using reduced sets then gives an additional improvement.



----

## References

[[1]](https://arxiv.org/abs/2104.06636)
A. Leitert:
Computing the Union Join and Subset Graph of Acyclic Hypergraphs in Subquadratic Time.
*arXiv* 2104.06636, 2021.

[[2]](https://link.springer.com/article/10.1007/PL00009272)
P. Pritchard:
A Fast Bit-Parallel Algorithm for Computing the Subset Partial Order.
*Algorithmica* 24, 76-86, 1999.
