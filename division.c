#include "division.h"
#include "validator.h"
#include "linearOps.h"
#include <string.h>
#include <math.h>

/* Implementation of algorithm 2 */
void algorithm2(Graph *G, double *divisionVector) {
	double *powerIterationResultVector, modularity, eigenValue;
	int gSize;

	gSize = G->currentSize;

	powerIterationResultVector = (double*)malloc(sizeof(double)*gSize);
	checkPointerForNull(powerIterationResultVector,
			"FAILED TO MALLOC POWER ITERATION VECTOR");
	powerIterationResultVector = powerIteration(G, powerIterationResultVector);

	eigenValue = calculateEigenValue(powerIterationResultVector, G);
	if (IS_POSITIVE(eigenValue)) {
		signTransformation(divisionVector, powerIterationResultVector, gSize);
		modularityMaximization(divisionVector, G)	;
		modularity = calculateModularity(divisionVector, G);
		if (!IS_POSITIVE(modularity)) {
			setVectorToTrivial(divisionVector, gSize);
		}
	}
	else {
		setVectorToTrivial(divisionVector, gSize);
	}
	free(powerIterationResultVector);
}

/* Implementation of algorithm 3, Returns number of clusters detected */
int algorithm3(GraphList *P, GraphList* O, SparseMatrix *motherA,
		int *kVector, int M) {
	int countGraphs, count1, count2, *indexVector1, *indexVector2, i,
	currGraphSize;
	double *divisionVector, *divisionVectorPtr;
	GraphNode *currentGraphNode, *g1Node, *g2Node;
	Graph *g1, *g2;

	countGraphs = 0;
	while (P->current_graph_node != NULL) {
			currentGraphNode = P->deque(P);
			currGraphSize = currentGraphNode->G->currentSize;
			divisionVector = (double*)malloc(sizeof(double)*(currGraphSize));
			checkPointerForNull(divisionVector,
					"FAILED TO MALLOC DIVISION VECTOR");

			algorithm2(currentGraphNode->G, divisionVector);
			divisionVectorPtr = divisionVector;
			count1 = 0;
			count2 = 0;
			for (i = 0; i < currentGraphNode->G->currentSize; i++) {
				if (*divisionVector == 1) {
					count1++;
				}
				else {
					count2++;
				}
				divisionVector++;
			}
			divisionVector = divisionVectorPtr;
			if (count1 == 0 || count2 == 0) {
				O->enque(O, currentGraphNode);
				countGraphs++;
			}
			else {
				indexVector1 = (int*)malloc(sizeof(int)*count1);
				checkPointerForNull(indexVector1,
						"FAILED TO MALLOC INDEX VECTOR 1");
				indexVector2  = (int*)malloc(sizeof(int)*count2);
				checkPointerForNull(indexVector2,
						"FAILED TO MALLOC INDEX VECTOR 2");

				divideGraph(currentGraphNode->G, divisionVector,
						indexVector1, indexVector2);
				currentGraphNode->G->freeGraph(currentGraphNode->G);
				free(currentGraphNode);
				g1 = graphCreate(motherA, indexVector1, count1, kVector, M);
				g2 = graphCreate(motherA, indexVector2, count2, kVector, M);

				g1Node = (GraphNode*)malloc(sizeof(GraphNode));
				checkPointerForNull(g1Node, "FAILED TO MALLOC G1 NODE");
				g1Node->G = g1;
				g1Node->next = NULL;

				g2Node = (GraphNode*)malloc(sizeof(GraphNode));
				checkPointerForNull(g2Node, "FAILED TO MALLOC G2 NODE");
				g2Node->G = g2;
				g2Node->next = NULL;

				if(count1 == 1){
					O->enque(O, g1Node);
					countGraphs++;
				}else{
					P->enque(P, g1Node);
				}
				if(count2 == 1){
					O->enque(O, g2Node);
					countGraphs++;
				}else{
					P->enque(P, g2Node);
				}
			}
			free(divisionVector);
		}
	return countGraphs;
}

/* Performs power iteration of given graph */
double* powerIteration(Graph *G, double *resultVector){
	double *b0, *tmp, *fVector;
	int legal, *kgVector, gSize, counter;

	gSize = G->currentSize;
	kgVector = G->kgVector;
	fVector = G->fVector;
	counter = 0;

	b0 = (double*)malloc(gSize*sizeof(double));
	checkPointerForNull(b0, "FAILED TO MALLOC B0");
	randomizeVector(gSize, b0);

	legal = 0;
	while (legal == 0) {
		G->Ag->mult(G->Ag, b0, resultVector);
		calculateKgPart(b0, kgVector, resultVector,G->M, gSize);
		calculateShiftFiPart(resultVector, b0, fVector, G->shift, gSize);

		divideVectorByK(resultVector,
				sqrt(scalarProduct(resultVector, resultVector, gSize)), gSize);
		legal = isDifferentByEpsilon(resultVector, b0, gSize);

		/* Swap pointers */
		tmp = b0;
		b0 = resultVector;
		resultVector = tmp;

		counter++;
		if (counter > 5000000) {
			printf("%s\n", "POWER ITERATION INFINITE LOOP DETECTED");
			exit(-1);
		}
	}
	free(b0);
	return resultVector;
}

/* Calculates modularity of division */
double calculateModularity(double *divisionVector, Graph *G) {
	int gSize, i;
	double *bgS, *ptr, sBgS;

	gSize = G->currentSize;
	bgS = (double*)malloc(sizeof(double)*gSize);
	checkPointerForNull(bgS, "FAILED TO MALLOC B_G_S");
	ptr = bgS;

	G->Ag->mult(G->Ag, divisionVector, bgS);
	calculateKgPart(divisionVector, G->kgVector, bgS, G->M, gSize);
	calculateShiftFiPart(bgS, divisionVector, G->fVector, 0, gSize);

	sBgS = 0.0;
	for (i = 0; i < gSize; i++) {
		sBgS += (*ptr) * (*divisionVector);
		ptr++;
		divisionVector++;
	}
	free(bgS);
	return sBgS;
}

/* Algorithm 4 implementation */
void modularityMaximization(double *divisionVector, Graph *G) {
	int prevMaxScoreIndex, driveByMaxScoreIndex, reachNull, nodeCol,
	*kgVectorStart, *kgVector, maxImproveIndex, *indicesStart, *unmovedStart,
	i, *unmoved, gSize, outIndex, *indices;
	double comparator, driveByMaxScore, kK, sK, *BsStart, *scoreStart , M,
	*score, *Bs, deltaQ, *improve, *improveStart, maxImprove, *sPtr;
	Node **arr, *curr_node;

	gSize = G->currentSize;
	arr = ((LinkedListStruct*)G->Ag->private)->nodeList;

	unmoved = (int*)calloc(gSize, sizeof(int));
	checkPointerForNull(unmoved, "FAILED TO MALLOC UNMOVED");
	unmovedStart = unmoved;

	indices = (int*)malloc(gSize*sizeof(int));
	checkPointerForNull(unmoved, "FAILED TO MALLOC INDICES");
	indicesStart = indices;

	improve = (double*)malloc(gSize*sizeof(double));
	checkPointerForNull(improve, "FAILED TO MALLOC IMPROVE");
	improveStart = improve;

	Bs = (double*)malloc(gSize*sizeof(double));
	checkPointerForNull(Bs, "FAILED TO MALLOC BS");
	BsStart = Bs;

	score = (double*)calloc(gSize,sizeof(double));
	checkPointerForNull(score, "FAILED TO MALLOC SCORE");
	scoreStart = score;

	kgVector = G->kgVector;
	kgVectorStart = kgVector;
	M = (double)G->M;
	sK = -70;

	do {
		for (outIndex = 0; outIndex < gSize; outIndex++) {
			driveByMaxScore = -0.5*HUGE_VAL;
			if(outIndex == 0) {
				sPtr = divisionVector;

				/* CALCULATING B[g]*s */
				G->Ag->mult(G->Ag, divisionVector, Bs);
				calculateKgPart(divisionVector, kgVector, Bs, M, gSize);
				for (i = 0; i < gSize; i++) {
					*score = -2*((*sPtr)*(*Bs) +
							((double)*kgVector*(double)*kgVector)/M);
					comparator = *score - driveByMaxScore;
					if (IS_POSITIVE(comparator)) {
						driveByMaxScore = *score;
						driveByMaxScoreIndex = i;
					}
					score++;
					sPtr++;
					kgVector++;
					Bs++;
				}
				score = scoreStart;
				kgVector = kgVectorStart;
				Bs = BsStart;
				sPtr = divisionVector;
			}
			else {
					kK = (double)kgVector[prevMaxScoreIndex];
					curr_node = arr[prevMaxScoreIndex];
					reachNull = 0;
					nodeCol = -1;
						for (i = 0; i < gSize; i++) {
							if (curr_node != NULL) {
								nodeCol = curr_node->column;
							}
							else {
								reachNull = 1;
							}
							if (i == prevMaxScoreIndex) {
								*score = -*score;
							}else if (*unmoved != 1) {
								if (reachNull == 0 && nodeCol == i ) {
									*score -= (4 * (*sPtr) * (sK) * (scoreCalculate(1, kK, *kgVector, M)));
									comparator = *score - driveByMaxScore;
									if (IS_POSITIVE(comparator)) {
										driveByMaxScore = *score;
										driveByMaxScoreIndex = i;
									}
									curr_node = curr_node->next;
								}
								else {
									*score -= (4 * (*sPtr) * (sK) * (scoreCalculate(0, kK, *kgVector, M)));
									comparator = *score - driveByMaxScore;
									if (IS_POSITIVE(comparator)) {
										driveByMaxScore = *score;
										driveByMaxScoreIndex = i;
									}
								}
							}
							else {
								if (nodeCol == i) {
									curr_node = curr_node->next;
								}
							}
							score++;
							kgVector++;
							sPtr++;
							unmoved++;
					}
					score = scoreStart;
					kgVector = kgVectorStart;
					sPtr = divisionVector;
					unmoved = unmovedStart;
			}
			score = scoreStart;
			sPtr = divisionVector;

			unmoved = unmovedStart;
			unmoved[driveByMaxScoreIndex] = 1;
			divisionVector[driveByMaxScoreIndex] *= -1;
			sK = divisionVector[driveByMaxScoreIndex];
			*indices = driveByMaxScoreIndex;
			indices++;

			prevMaxScoreIndex = driveByMaxScoreIndex;

			if (outIndex == 0) {
				*improve = driveByMaxScore;
				maxImprove = driveByMaxScore;
				maxImproveIndex = outIndex;
			}
			else {
				*improve = *(improve-1) + driveByMaxScore;
				comparator = *improve - maxImprove;
				if (IS_POSITIVE(comparator)) {
					maxImprove = *improve;
					maxImproveIndex = outIndex;
				}
			}
			improve++;
		}
		indices -= 1;
		for (i = gSize-1; i > maxImproveIndex; i--) {
			divisionVector[*indices] *= -1;
			indices--;
		}

		if (maxImproveIndex == gSize-1) {
			deltaQ = 0.0;
		}
		else {
			deltaQ = maxImprove;
		}
		unmoved = unmovedStart;
		memset(unmoved, 0, gSize*sizeof(int));
		improve = improveStart;
		memset(improve, 0.0, gSize*sizeof(double));
		indices = indicesStart;
		memset(indices, 0, gSize*sizeof(int));
		}
		while (IS_POSITIVE(deltaQ));
		score = scoreStart;
		Bs = BsStart;
		free(unmoved);
		free(improve);
		free(indices);
		free(Bs);
		free(score);
	}

/* Calculates current score */
double scoreCalculate(int Aij, double kK, int currK, int M){
	double result, Dij;

	Dij = ((double)currK * kK )/(double)M;
	result = ((double)Aij - Dij);

	return result;
}

/* Calculates eigen value minus shift */
double calculateEigenValue(double *bk, Graph *G) {
	double eigenValue, *result, product;
	int gSize;

	gSize = G->currentSize;
	result = (double*)malloc(gSize*sizeof(double));
	checkPointerForNull(result, "FAILED TO MALLOC RESULT");

	G->Ag->mult(G->Ag, bk, result);
	calculateKgPart(bk, G->kgVector, result, G->M, gSize);
	calculateShiftFiPart(result, bk, G->fVector, G->shift, gSize);
	product = scalarProduct(bk, bk, gSize);
	if (!IS_POSITIVE(product)) {
		printf("%s\n", "EIGEN VALUE PRODUCT IS 0");
		exit(-1);
	}
	eigenValue = scalarProduct(result, bk, gSize) / product;
	free(result);
	return eigenValue - (G->shift);
}

/* Calculates shift (NORM1) AND fi part of given row of B[g] */
void calculateShiftFiPart(double *resultVector, double *b0, double *fVector,
		double shift, int gSize) {
	int i;

	for (i = 0; i < gSize; i++) {
		*resultVector += ((-(*fVector) + shift) * (*b0));
		resultVector++;
		fVector++;
		b0++;
	}
}

/* Calculates K part of given row of B[g] */
void calculateKgPart(double *b0 , int *kgVector, double *resultVector,
		int M, int gSize) {
	int i, *kPtr;
	double product;

	product = 0.0;
	kPtr = kgVector;

	for (i = 0; i < gSize; i++) {
		product += ((double)*kgVector) * (*b0);
		kgVector++;
		b0++;
	}

	for (i = 0; i < gSize; i++) {
		*resultVector -= ((((double)*kPtr) / (double)M) * product);
		kPtr++;
		resultVector++;
	}
}

/* Creates 2 Index vectors for 2 sub graphs using devision vector */
void divideGraph(Graph *G, double *divisionVector,
		int *indexVector1, int *indexVector2) {
	int i, *ptr;

	ptr = G->indexVector;
	for (i = 0; i < G->currentSize; i++) {
		if (*divisionVector == 1) {
			*indexVector1 = *(ptr);
			indexVector1++;
		}
		else {
			*indexVector2 = *(ptr);
			indexVector2++;
		}
		ptr++;
		divisionVector++;
	}
}

/* Creating output file */
GraphNode* create_output(GraphList *O, FILE *outputFile, int countGraphs) {
	int currentSize, checkSize;
	GraphNode *firstOut;

	firstOut = O->current_graph_node;

	checkSize = fwrite(&countGraphs, sizeof(int), 1, outputFile);
	checkSizeForTarget(checkSize, 1 ,
			"FAILED TO WRITE COUNT GRAPHS TO OUTPUT FILE");

	while (O->current_graph_node != NULL) {
		currentSize = O->current_graph_node->G->currentSize;
		checkSize = fwrite(&currentSize, sizeof(int), 1, outputFile);
		checkSizeForTarget(checkSize, 1,
				"FAILED TO WRITE CURRENT SIZE TO OUTPUT FILE");

		checkSize = fwrite(O->current_graph_node->G->indexVector, sizeof(int),
				currentSize, outputFile);
		checkSizeForTarget(checkSize, currentSize,
				"FAILED TO WRITE INDEX VECTOR TO OUTPUT FILE");

		O->current_graph_node = O->current_graph_node->next;
	}
	return firstOut;
}
