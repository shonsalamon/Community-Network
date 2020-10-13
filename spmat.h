#ifndef _SPMAT_H
#define _SPMAT_H

#include <stdio.h>

/* Sparse matrix module that describes the spmat struct. */

/* Struct to describe a node inside adj spmat */
typedef struct _Node {
	int column;
    struct _Node *next;
} Node;

/* Struct to describe the contents of private pointer on spmat struct.
 * An array of nodes. */
typedef struct _LinkedListStruct {
	Node **nodeList;
} LinkedListStruct;

/* A sparse matrix implementation */
typedef struct _SparseMatrix {
	/* Matrix size (n*n) */
	int		n;

	/* Adds row i the matrix. Called before any other call,
	 * exactly n times in order (i = 0 to n-1) */
	/* CHANGED IT TO INT IN ORDER TO ACCOMPANY THE 1s AND 0s of the A Matrix */
	void	(*addRow)(struct _SparseMatrix *A, const int *row);

	/* Frees all resources used by A */
	void	(*free)(struct _SparseMatrix *A);

	/* Multiplies matrix A by vector v, into result vector */
	void	(*mult)(const struct _SparseMatrix *A, const double *v,
			double *result);

	/* Private field for inner implementation.
	 * Should not be read or modified externally */
	void	*private;

} SparseMatrix;

/* Allocates a new linked-lists sparse matrix of size n */
SparseMatrix* spmatAllocateList(int n);

/* Adds a row to spmat */
void addRowLinkedList(struct _SparseMatrix *A, const int *row);

/* Frees all resources of spmat */
void freeLinkedList(struct _SparseMatrix *A);

/* Multiplies spmat with a vector */
void multLinkedList(const struct _SparseMatrix *A, const double *v,
		double *result);

/* Read input graph into mother_A adj matrix using spmat */
void readInputIntoMotherA(SparseMatrix* motherA, FILE *inputFile, int n,
		int *kVector, int *M);

#endif
