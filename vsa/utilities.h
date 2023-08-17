#include <stdio.h>

/* PRINTS ONLY FAILURES: */
/*#define TESTS(x) if((x) == 0) { printf("FAILURE: file %s line %d\n", __FILE__, __LINE__); };*/

/* PRINTS BOTH SUCCESS AND FAILURE: */
/*#define TESTS(x) ((x) == 0) ? printf("FAILURE: file %s line %d\n", __FILE__, __LINE__) : printf("SUCCESS\n");*/

/* PRINTS BOTH SUCCESS AND FAILURE: 
example, this will evaluate to true and print success: 
TESTS(161 == (VsaLargestChunk(my_vsa))); */
#define TESTS(x) (x) ? printf("SUCCESS\n") : printf("\033[0;31mFAILURE: file %s line %d\033[0m\n", __FILE__, __LINE__) ;
