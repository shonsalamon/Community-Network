#include "validator.h"
#include <stdio.h>
#include <stdlib.h>

/* Check NULL for pointers */
void checkPointerForNull(void *ptr, char *msg) {
	if (ptr == NULL) {
		printf("%s\n", msg);
		exit(-1);
	}
}

/* Check value of number, Displays custom message and exits if true */
void checkSizeForTarget(int toCheck, int target , char *msg) {
	if (toCheck != target) {
		printf("%s\n", msg);
		exit(-1);
	}
}
