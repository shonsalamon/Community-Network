#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "spmat.h"
#include "validator.h"

/* Allocates a new linked-lists sparse matrix of size n */
SparseMatrix* spmatAllocateList(int n){
	SparseMatrix *s;
	LinkedListStruct *listStruct;

	s = (SparseMatrix*)malloc(sizeof(SparseMatrix));
	checkPointerForNull(s, "FAILED TO MALLOC SPMAT");

	listStruct = (LinkedListStruct*)malloc(sizeof(LinkedListStruct));
	checkPointerForNull(listStruct, "FAILED TO MALLOC LIST STRUCT");

	s->n = n;
	s->addRow = &addRowLinkedList;
	s->mult = &multLinkedList;
	s->free = &freeLinkedList;
	s->private = (LinkedListStruct*)listStruct;

	listStruct->nodeList = (Node**)calloc((n+1),sizeof(Node*));
	checkPointerForNull(listStruct->nodeList, "FAILED TO MALLOC NODE LIST");

	return s;
}

/* Adds a row to spmat */
void addRowLinkedList(struct _SparseMatrix *A, const int *row){
	int column, isFirst;
	Node *newNode, *currentNode;
	LinkedListStruct *listStruct;

	isFirst = 1;
	listStruct = (LinkedListStruct*)A->private;

	for(column = 0; column < A->n; column++){
		if(*row != 0){
			newNode = (Node*)malloc(sizeof(Node));
			checkPointerForNull(newNode, "FAILED TO MALLOC NODE");

			newNode->column = column;
			newNode->next = NULL;

			if(isFirst == 1){
				*(listStruct->nodeList) = newNode;
				isFirst = 0;
				currentNode = *(listStruct->nodeList);
			}
			else{
				currentNode->next = newNode;
				currentNode = currentNode->next;
			}
		}
		row++;
	}
	listStruct->nodeList++;
}

/* Frees all resources of spmat */
void freeLinkedList(struct _SparseMatrix *A){
	Node *current, *nextNode, **arr;
	int i;
	LinkedListStruct *listStruct;

	listStruct = (LinkedListStruct*)A->private;
	arr = listStruct->nodeList;

	for(i = 0; i < A->n; i++){
		current = *arr;
		if(current != NULL){
			nextNode = current->next;
			while(nextNode != NULL){
				free(current);
				current = nextNode;
				nextNode = nextNode->next;
			}
			free(current);
		}
		arr++;
	}
	free(listStruct->nodeList);
	free(listStruct);
	free(A);
}

/* Multiplies spmat with a vector */
void multLinkedList(const struct _SparseMatrix *A, const double *v,
		double *result) {
	int i;
	Node *current, **arr;
	double dotProduct;
	LinkedListStruct *listStruct;

	listStruct = (LinkedListStruct*)A->private;
	arr = listStruct->nodeList;
	dotProduct = 0;
	current = *arr;

	for(i = 0; i < A->n; i++){
		while(current != NULL){
			dotProduct += (double)(v[current->column]);
			current = current->next;
		}
		arr++;
		current = *arr;
		*result = dotProduct;
		result++;
		dotProduct = 0.0;
	}
}

/* Read input graph into mother_A adj matrix using spmat */
void readInputIntoMotherA(SparseMatrix* motherA, FILE *inputGraphFile, int n,
		int *kVector, int *M) {
	int checkSize, *vertexVector, *insertVector, i, j, currEdgesCount,
	prevIndex, currentIndex, *startVertex, *startInsert;
	LinkedListStruct *motherListStruct;
	Node **startOfMotherNodeList;

	motherListStruct = (LinkedListStruct*)motherA->private;
	startOfMotherNodeList = motherListStruct->nodeList;

	vertexVector = (int*)malloc(n*sizeof(int));
	checkPointerForNull(vertexVector, "FAILED TO MALLOC VERTEX VECTOR");

	/* calloc initializes to 0 */
	insertVector = (int*)calloc(n, sizeof(int));
	checkPointerForNull(insertVector, "FAILED TO MALLOC INSERT VECTOR");

	startVertex = vertexVector;
	startInsert = insertVector;

	for (i = 0; i < n; i++) {
		prevIndex = 0;
		currentIndex = 0;

		/* Get number of edges for vertex i */
		checkSize = fread(&currEdgesCount, sizeof(int), 1, inputGraphFile);
		checkSizeForTarget(checkSize, 1, "FAILED TO READ CURRENT EDGES COUNT");

		*kVector = currEdgesCount;
		kVector++;
		*M += currEdgesCount;

		/* Read connected vertexes */
		checkSize = fread(vertexVector, sizeof(int), currEdgesCount,
				inputGraphFile);
		checkSizeForTarget(checkSize, currEdgesCount,
		"FAILED TO WRITE INTO VERTEX VECTOR or READ FROM INPUT GRAPH FILE");

		for (j = 0; j < currEdgesCount; j++) {
			currentIndex = *vertexVector;
			insertVector += currentIndex - prevIndex;
			*insertVector = 1;
			vertexVector++;
			prevIndex = currentIndex;
		}

		insertVector = startInsert;
		vertexVector = startVertex;

		motherA->addRow(motherA, insertVector);

		/* resetting vector to 0's */
		memset(insertVector, 0, n*sizeof(int));
	}
	if (*M == 0) {
		printf("M EQUALS TO 0");
		exit(-1);
	}
	motherListStruct->nodeList = startOfMotherNodeList;
	free(vertexVector);
	free(insertVector);
}
