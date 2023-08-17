#include <stdio.h>	/* printf */
#include <stdlib.h>	/* malloc */
#include "vsa.h"
#include "utilities.h"

int main()
{
	/* test case 1 - aligned address */
	int allocation_size1 = 200;
	vsa_t *my_vsa1 = NULL;
	char *address1 = malloc(allocation_size1);
	void *allocated_address1 = NULL;
	void *allocated_address2 = NULL;
	void *allocated_address3 = NULL;
	void *allocated_address4 = NULL;
	
	/* test case 2 - not aligned address */
	int allocation_size2 = 100;
	vsa_t *my_vsa2 = NULL;
	char *address2 = malloc(allocation_size2);
	void *allocated_address10 = NULL;
	void *allocated_address20 = NULL;


	/********** TEST CASE 1 - ALIGNED ADDRESS **********/
	printf("********** TEST CASE 1 - ALIGNED ADDRESS **********\n\n");

	printf("allocated address: %p", address1);
	
	/***** VsaInit *****/
	printf("\n\n----- VsaInit -----\n\n");
	my_vsa1 = VsaInit(address1, allocation_size1);	
	TESTS(161 != (VsaLargestChunk(my_vsa1)));	
	TESTS(160 == (VsaLargestChunk(my_vsa1)));
	
	
	/***** VsaAlloc *****/
	printf("\n\n----- VsaAlloc -----\n\n");
	allocated_address1 = VsaAlloc(my_vsa1, 10);
	TESTS(112 == (VsaLargestChunk(my_vsa1)));
	
	allocated_address2 = VsaAlloc(my_vsa1, 20);
	TESTS(56 == (VsaLargestChunk(my_vsa1)));
	
	allocated_address3 = VsaAlloc(my_vsa1, 5);
	TESTS(16 == (VsaLargestChunk(my_vsa1)));
	
	allocated_address4 = VsaAlloc(my_vsa1, 7);
	TESTS(0 == (VsaLargestChunk(my_vsa1)));
	
	/* make sure addresses are aligned */
	printf("\n");
	TESTS(0 == ((size_t)allocated_address1 - (size_t)address1) % 8);
	TESTS(0 == ((size_t)allocated_address2 - (size_t)allocated_address1) % 8);
	TESTS(0 == ((size_t)allocated_address3 - (size_t)allocated_address2) % 8);
	TESTS(0 == ((size_t)allocated_address4 - (size_t)allocated_address3) % 8);
	
	
	/***** VsaFree *****/
	printf("\n\n----- VsaFree -----\n\n");
	VsaFree(allocated_address2);
	TESTS(24 == (VsaLargestChunk(my_vsa1)));
	TESTS(NULL != allocated_address2);
	
	VsaFree(allocated_address3);
	TESTS(64 == (VsaLargestChunk(my_vsa1)));
	TESTS(NULL != allocated_address3);
	
	
	/***** VsaAlloc *****/
	printf("\n\n----- VsaAlloc -----\n\n");
	allocated_address2 = VsaAlloc(my_vsa1, 20);
	TESTS(8 == (VsaLargestChunk(my_vsa1)));
	TESTS(NULL != allocated_address2);
	
	/* allocation impossible */
	allocated_address3 = VsaAlloc(my_vsa1, 9);
	TESTS(8 == (VsaLargestChunk(my_vsa1)));
	TESTS(NULL == allocated_address3);



	/********** TEST CASE 2 - NOT ALIGNED ADDRESS **********/
	printf("\n\n\n********** TEST CASE 2 - NOT ALIGNED ADDRESS **********\n\n");

	printf("allocated address: %p\n", address2);
	printf("allocated address: %p", address2+3);
	
	/***** VsaInit *****/
	printf("\n\n----- VsaInit -----\n\n");
	my_vsa2 = VsaInit(address2+3, allocation_size2);	
	TESTS(161 != (VsaLargestChunk(my_vsa2)));	
	TESTS(55 == (VsaLargestChunk(my_vsa2)));
	
	
	/***** VsaAlloc *****/
	printf("\n\n----- VsaAlloc -----\n\n");
	allocated_address10 = VsaAlloc(my_vsa2, 7);
	TESTS(15 == (VsaLargestChunk(my_vsa2)));
	
	/* allocation impossible */
	allocated_address20 = VsaAlloc(my_vsa2, 9);
	TESTS(15 == (VsaLargestChunk(my_vsa2)));
	TESTS(NULL == allocated_address20);
	
	/* make sure addresses are aligned */
	printf("\n");
	TESTS(0 == ((size_t)allocated_address10 - (size_t)address2) % 8);
	TESTS(0 == ((size_t)allocated_address20 - (size_t)allocated_address10) % 8);
	
	
	/***** VsaFree *****/
	printf("\n\n----- VsaFree -----\n\n");
	VsaFree(allocated_address10);
	TESTS(55 == (VsaLargestChunk(my_vsa2)));
	
	
	/***** VsaAlloc *****/
	printf("\n\n----- VsaAlloc -----\n\n");
	allocated_address20 = VsaAlloc(my_vsa2, 15);
	TESTS(0 == (VsaLargestChunk(my_vsa2)));
	
	/* allocation impossible */
	allocated_address10 = VsaAlloc(my_vsa2, 9);
	TESTS(0 == (VsaLargestChunk(my_vsa2)));
	TESTS(NULL == allocated_address10);


	free(address1);
	free(address2);
	
	return 0;
}

