#ifndef VALIDATOR_H_
#define VALIDATOR_H_

/* Validating module for checking pointers and sizes. */

/* Check for NULL pointers */
void checkPointerForNull(void *ptr, char *msg);

/* Check value of number, Displays custom message and exits if true */
void checkSizeForTarget(int toCheck, int target , char *msg);

#endif /* VALIDATOR_H_ */
