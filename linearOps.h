#ifndef LINEAROPS_H_
#define LINEAROPS_H_

#define IS_POSITIVE(X) ((X) > 0.00001)

/* Linear operations and transformations module */

/* sets vector to 0....N-1 */
void createInitialIndexVector(int *vector, int size);

/* Randomizing a vector */
void randomizeVector(int n, double* vector);

/* Applies sign transformation  */
void signTransformation(double *divisionVector, double *resultVector,
		int gSize);

/* Calculates scalar product of 2 vectors */
double scalarProduct(double *vector1, double *vector2, int n);

/* Divide vector by a factor of K */
void divideVectorByK(double *vector, double K, int N);

/* Checks difference of epsilon defined */
int isDifferentByEpsilon(double *vector1, double *vector2, int N);

/* Sets vector to 1s */
void setVectorToTrivial(double *vector, int size);

#endif /* LINEAROPS_H_ */
