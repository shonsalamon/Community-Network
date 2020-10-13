#include "glist.h"
#include "graph.h"
#include "spmat.h"
#include "division.h"
#include "validator.h"
#include "linearOps.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

int main(int argc, char* argv[]) {
	FILE *inputFile, *outputFile;
	SparseMatrix *motherA;
	char *inputFilePath, *outputFilePath;
	int checkSize, countGraphs, n, *kVector, M, *initialIndexVector;
	GraphList *O, *P;
	Graph *motherG;
	GraphNode *motherNode, *firstOutNode;

	checkSizeForTarget(argc, 3, "ARGC IS NOT 3");

	/* For the randomizing */
	srand(time(NULL));

	/* Starting the run with creating the "home" graph A */
	inputFilePath = argv[1];
	inputFile = fopen(inputFilePath, "rb");
	checkPointerForNull(inputFile, "INPUT FILE READ FAILED");

	outputFilePath = argv[2];
	outputFile = fopen(outputFilePath, "wb");
	checkPointerForNull(outputFile, "OUTPUT FILE READ FAILED");

	/* Reading N = size of graph */
	checkSize = fread(&n, sizeof(int), 1, inputFile);
	if (checkSize != 1 || n < 1) {
		printf("%s\n", "CHECKING GRAPH SIZE FAILED or GRAPH SIZE < 1");
		exit(-1);
	}

	/* Rank vector */
	kVector = (int*)malloc(n*sizeof(int));
	checkPointerForNull(kVector, "FAILED TO MALLOC K_VECTOR");

	M = 0;
	/* Setting mother_A */
	motherA = spmatAllocateList(n);
	readInputIntoMotherA(motherA, inputFile, n, kVector, &M);
	fclose(inputFile);

	initialIndexVector = (int*)malloc(sizeof(int)*n);
	createInitialIndexVector(initialIndexVector, n);

	motherG = graphCreate(motherA, initialIndexVector, n, kVector, M);
	motherNode = (GraphNode*)malloc(sizeof(GraphNode));
	checkPointerForNull(motherNode, "FAILED TO MALLOC MOTHER NODE");

	motherNode->G = motherG;
	motherNode->next = NULL;

	/* ALGORITHM 3 STARTS HERE
	   CREATING O, P G-LISTS */
	P = allocateGraphList();
	O = allocateGraphList();
	P->current_graph_node = NULL;
	O->current_graph_node = NULL;
	P->enque(P, motherNode);

	countGraphs = algorithm3(P, O, motherA, kVector, M);

	firstOutNode = create_output(O, outputFile, countGraphs);
	fclose(outputFile);

	O->current_graph_node = firstOutNode;
	O->free_glist(O);
	P->free_glist(P);

	free(kVector);
	motherA->free(motherA);

	return 0;
}
