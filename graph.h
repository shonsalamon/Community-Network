#ifndef GRAPH_H_
#define GRAPH_H_

#include "spmat.h"

/* Graph representation module.
 * Contains functions that calculates every aspect of BHat[g] matrix. */

/*
 * Struct to represent a graph
 * Takes a adjacency matrix A (sparse matrix),
 * number of nodes n as input and represents a graph
 */
typedef struct _Graph {

	/* Indexes vector - g length vector */
	int *indexVector;

	/* Current number of vertices */
	int currentSize;

	/* ki vector */
	int *kVector;

	/* Sum of ranks -> sum of rank vector */
	int M;

	/* A[g] */
	SparseMatrix *Ag;

	/* Reduced k_vector to relevant indexes */
	int *kgVector;

	/* fi vector for Power Iteration */
	double *fVector;

	/* Shift value for Power Iteration */
	double shift;

	void (*freeGraph)(struct _Graph *G);

} Graph;

/* Creates graph using index vector and mother_A */
Graph* graphCreate(SparseMatrix* motherA, int *indexVector,
		int indexVectorSize, int *kVector, int M);

/* Creates adj matrix using mother_A and index vector */
void createAg(Graph *G , SparseMatrix *motherA, SparseMatrix *Ag,
		int *indexVector, int gSize, int *kgVector, double *fVector);

/* Extracts relevant elements from original k_vector */
void extractKgVector(int *kVector, int *kgVector, int gSize, int *indexVector);

/* Calculates fi AND shift WHILE create_Ag is running */
double calculateFiAndShift(int *kgVector, int i, int gSize, int *row,
		double sumOfRowIofA, double *fVector, int M);

/* Frees all resources of graph */
void freeGraph(Graph *G);

#endif /* GRAPH_H_ */
