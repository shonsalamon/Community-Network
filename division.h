#ifndef DIVISION_H_
#define DIVISION_H_

#include "glist.h"
#include "graph.h"
#include "spmat.h"
#include <stdio.h>
#include <stdlib.h>

/* Division module.
 * Contains all functionality of the recursive division process.
 * algorithm3 is called from main. motherG is the first node of GraphList P,
 * And the procedure proceeds,
 * until final division is formed and contained inside O. */

/* Implementation of algorithm 2 */
void algorithm2(Graph *G, double *divisionVector);

/* Implementation of algorithm 3, Returns number of clusters detected */
int algorithm3(GraphList *P, GraphList* O, SparseMatrix *motherA,
		int *kVector, int M);

/* Performs power iteration of given graph */
double* powerIteration(Graph *G, double *resultVector);

/* Calculates modularity of division */
double calculateModularity(double *divisionVector, Graph *G);

/* Algorithm 4 implementation */
void modularityMaximization(double *divisionVector, Graph *G);

/* Calculates current score */
double scoreCalculate(int Aij, double kK, int currK, int M);

/* Calculates eigen value minus shift */
double calculateEigenValue(double *bK, Graph *G);

/* Calculates shift (NORM1) AND fi part of given row of B[g] */
void calculateShiftFiPart(double *resultVector, double *b0, double *fVector,
		double shift, int gSize);

/* Calculates K part of given row of B[g] */
void calculateKgPart(double *b0 , int *kgVector, double *resultVector,
		int M, int gSize);

/* Creates 2 Index vectors for 2 sub graphs using devision vector */
void divideGraph(Graph *G, double *divisionVector,
		int *indexVector1, int *indexVector2);

/* Creating output file */
GraphNode* create_output(GraphList *O, FILE *outputFile, int countGraphs);

#endif /* DIVISION_H_ */
