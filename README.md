# community-network

In complex networks, a network is said to have community structure if the nodes of the
network can be grouped into groups of nodes with dense connections internally, and sparser
connections between groups

# Project Description
This project contains implementation of the algorithm for detecting community structures (or clusters) in a network.
The ability to detect such groups is of significant importance. For example, partitioning a protein-protein interaction network into clusters can provide a modular view of the network, with different groups of nodes performing different functions within the cell.
Project's finale grade: 100


# The Mathematical Basis

We represent a network by a graph G = (V, E), and let A be the adjacency matrix of G
![alt tag](https://github.com/shonsalamon/community-network/blob/main/Images/A%20definition.JPG)




A given group of vertices in a network is considered a community if the number of edges
within the group is significantly more than expected (by chance). We define the modularity
of a group as the number of edges within the group minus the expected number of edges in
a random graph with the same degrees
![alt tag](https://github.com/shonsalamon/community-network/blob/main/Images/Q%20definition.JPG)


# project's goal 
Our goal in this project is thus to find a division that maximizes the modularity, that
is, a division of the graph into groups such that the modularity Q, the sum of all group
modularities, is close to maximal.

# File Format
The input and output files are both binary files consisting only of integers.
* _**input file**_ :
The first value represents the number of nodes in the network, n = |V|. 
The second value represents the number of edges of the first node, i.e., k1. It is followed by
the k1 indices of its neighbors, in increasing order.
The next value is k2, followed by the k2 indices of the neighbors of the second node, then k3
and its k3 neighbors, and so on until node n.

* _**output file**_ :
The first value represents the number of groups in the division.
The second value represents the number of nodes in the first group, followed by the indices
of the nodes in the group, in increasing order.
The next value is the number of nodes in the second group, followed by the indices of the
nodes in group, then the number of nodes and indices of nodes in the third group, and so
on until the last group.


