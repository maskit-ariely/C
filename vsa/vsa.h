#ifndef VSA_H
#define VSA_H

#include <stddef.h>	/* size_t */

/*  */
typedef struct vsa vsa_t;

/* initializes a VSA for efficient memory management of variable-sized blocks */
vsa_t *VsaInit (void *alloc_dest, size_t size);

/* allocates a memory block of the specified size from the VSA */
void *VsaAlloc(vsa_t *vsa, size_t block_size);

/* takes a pointer to a memory block and marks it as free, making it available for future allocations */
void VsaFree(void *block);

/* returns the size of the largest available memory chunk */
size_t VsaLargestChunk(vsa_t *vsa);

#endif /* VSA_H */
