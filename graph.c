#include "graph.h"
#include "validator.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Creates graph using index vector and mother_A */
Graph* graphCreate(SparseMatrix *motherA, int *indexVector,
		int indexVectorSize, int *kVector, int M) {
	Graph *G;
	int *kg_Vector;
	SparseMatrix *Ag;
	double *fVector;

	G = (Graph*)malloc(sizeof(Graph));
	checkPointerForNull(G, "FAILED TO MALLOC GRAPH");

	/* Calculating Ki of G vector */
	kg_Vector = (int*)malloc(indexVectorSize*sizeof(int));
	checkPointerForNull(kg_Vector, "FAILED TO MALLOC K_G_VECTOR");
	extractKgVector(kVector, kg_Vector, indexVectorSize, indexVector);

	fVector = (double*)malloc(indexVectorSize*sizeof(double));
	checkPointerForNull(fVector, "FAILED TO MALLOC F VECTOR");

	Ag = spmatAllocateList(indexVectorSize);
	Ag->n = indexVectorSize;

	G->indexVector = indexVector;
	G->currentSize = indexVectorSize;
	G->kVector = kVector;
	G->M = M;
	G->freeGraph = &freeGraph;

	/* Actually create A_g */
	createAg(G , motherA, Ag, indexVector, indexVectorSize, kg_Vector,
			fVector);

	G->kgVector = kg_Vector;
	G->fVector = fVector;
	G->Ag = Ag;

	return G;
}

/* Creates adj matrix using mother_A and index vector */
void createAg(Graph *G , SparseMatrix *motherA, SparseMatrix *Ag,
		int *indexVector, int gSize, int *kgVector, double *fVector) {
	int i, j, *indexVectorPtr, *row, *rowStart, currentRow, prevRow,
	*indexVectorStart, rowiofA;
	LinkedListStruct *motherListStruct, *AgListStruct;
	Node *currNode, **nodeListMotherA, **startOfAgNodeList;
	double maxShift, currentShift;

	/* reads the private into the list struct */
	motherListStruct = (LinkedListStruct*)motherA->private;
	nodeListMotherA = motherListStruct->nodeList;

	AgListStruct = (LinkedListStruct*)Ag->private;
	startOfAgNodeList = AgListStruct->nodeList;

	row = (int*)calloc(gSize, sizeof(int));
	checkPointerForNull(row, "FAILED TO MALLOC ROW");

	indexVectorStart = indexVector;
	rowiofA = 0;
	currentShift = 0.0;
	maxShift = 0.0;
	prevRow = 0;
	rowStart = row;
	indexVectorPtr = indexVector;

	for (i = 0; i < gSize; i++) {
		currentRow = *indexVectorPtr;
		nodeListMotherA += currentRow - prevRow;
		currNode = *nodeListMotherA;
		j = 0;
		while (currNode != NULL && j < gSize) {
			if (currNode->column == *indexVector) {
				/* this is used to calculate fi.
				 * This is the sum of row i of A: Sum[j] Aij */
				rowiofA++;
				*row = 1;
				currNode = currNode->next;
				indexVector++;
				j++;
				row++;
			}
			else if (currNode->column < *indexVector) {
				currNode = currNode->next;
			}
			else {
				indexVector++;
				j++;
				row++;
			}
		}
		/* THIS IS CALCULATING fi coordinate of f_vector */
		/* THIS IS CALCULATING Ki*Kj/M,
		 * and subtracting it from sum of row i of A[g]
		 */
		row = rowStart;
		currentShift = calculateFiAndShift(kgVector, i, gSize, row,
				(double)rowiofA, fVector, G->M);
		if (currentShift > maxShift) {
			maxShift = currentShift;
		}

		fVector++;
		rowiofA = 0;

		Ag->addRow(Ag, row);

		/* Resetting vector to 0's */
		memset(row, 0, gSize*sizeof(int));
		indexVector = indexVectorStart;
		indexVectorPtr++;
		prevRow = currentRow;
	}

	G->shift = maxShift;

	/* Reset pointer */
	AgListStruct->nodeList = startOfAgNodeList;
	free(row);
}

/* Extracts relevant elements from original k_vector */
void extractKgVector(int *kVector, int *kgVector, int gSize,
		int *indexVector) {
	int i, currentIndex, prevIndex;

	prevIndex = 0;
	for (i = 0; i < gSize; i++) {
		currentIndex = *indexVector;
		kVector += currentIndex - prevIndex;
		*kgVector = *kVector;
		indexVector++;
		kgVector++;
		prevIndex = currentIndex;
	}
}

/* Calculates fi AND shift WHILE create_Ag is running */
double calculateFiAndShift(int *kgVector, int i, int gSize, int *row,
		double sumOfRowIofA, double *fVector, int M) {
	int j;
	double ki, totalKg, shiftI, currentKj, fi, doubleM;

	doubleM = (double)M;
	shiftI = 0.0;
	totalKg = 0.0;
	ki = (double)kgVector[i];

	for (j = 0; j < gSize; j++) {
		currentKj = (ki* ((double)*kgVector)) / doubleM;
		totalKg += currentKj; /* for the fi calculation */
		if (j != i) {
			shiftI += fabs((double)*row - currentKj);
		}
		kgVector++;
		row++;
	}

	fi = sumOfRowIofA - totalKg;
	*fVector = fi;
	shiftI += fabs(-((ki*ki)/doubleM) - fi);

	return shiftI;
}

/* Frees all resources of graph */
void freeGraph(Graph *G) {
	free(G->kgVector);
	free(G->fVector);
	free(G->indexVector);
	G->Ag->free(G->Ag);
	free(G);
}
