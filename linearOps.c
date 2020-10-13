#include "linearOps.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* sets vector to 0....N-1 */
void createInitialIndexVector(int *vector, int size) {
	int i;

	for (i = 0; i < size; i++) {
		*vector = i;
		vector++;
	}
}

/* Randomizing a vector */
void randomizeVector(int n, double* vector){
	int i;

	for(i = 0; i < n; i++){
		*vector = (double)rand();
		vector++;
	}
}

/* Applies sign transformation  */
void signTransformation(double *divisionVector, double *resultVector,
		int gSize) {
	int i;

	for (i = 0; i < gSize; i++) {
		if (IS_POSITIVE(*resultVector)) {
			*divisionVector = 1;
		}
		else {
			*divisionVector = -1;
		}
		divisionVector++;
		resultVector++;
	}
}

/* Calculates scalar product of 2 vectors */
double scalarProduct(double *vector1, double *vector2, int n) {
	int i;
	double product;

	product = 0.0;
	for (i = 0; i < n; i++) {
		product += (*vector1)*(*vector2);
		vector1++;
		vector2++;
	}
	return product;
}

/* Divide vector by a factor of K */
void divideVectorByK(double *vector, double K, int N){
	int i;

	if (!IS_POSITIVE(K)) {
		printf("%s", "DENOMINATOR IS 0");
		exit(-1);
	}
	for(i = 0; i < N; i++){
		*vector = (double)(*vector / K);
		vector++;
	}
}

/* Checks difference of epsilon defined */
int isDifferentByEpsilon(double *vector1, double *vector2, int N) {
	int i;
	double result;

	for(i = 0; i < N; i++){
		result = fabs(*vector1 - *vector2);
		if(IS_POSITIVE(result)){
			return 0;
		}
		vector1++;
		vector2++;
	}
	return 1;
}

/* Sets vector to 1s */
void setVectorToTrivial(double *vector, int size) {
	int i;

	for (i = 0; i < size; i++) {
		*vector = 1;
		vector++;
	}
}
