#include <assert.h>
#include "vsa.h"

#define COOKIE 0xDEADBEEF
#define WORD_SIZE sizeof(long)	/* system's word size for memory alignment */

typedef struct header header_t;

/*
 * Struct:  vsa 
 * --------------------
 *  represents the Variable Size Allocator (VSA)
 *
 *  first_header: pointer to the first header in the VSA memory region, used as a handle for VSA operations
 */
struct vsa
{
	size_t *first_header;
};

/*
 * Struct:  header 
 * --------------------
 *  represents the header structure for each memory block in the VSA
 *
 *  this struct is placed at the beginning of each memory block to manage its metadata
 *
 *  is_free:     a flag indicating whether the memory block is free (1) or in use (0).
 *  next_block:  a pointer to the next header in the VSA memory region (NULL if it's the last block)
 *  block_size:  total size (in bytes) of the memory block, including the header
 *  cookie:      a constant value used for integrity checks to detect memory corruption
 */
struct header
{
	size_t is_free;	
	void *next_block;
	size_t block_size;	
	size_t cookie;
};


/*
 * Function:  Defragmentation 
 * --------------------
 *  defragments the VSA by merging adjacent free memory blocks
 *
 *  this function iterates through the headers to identify adjacent free memory blocks and merges them
 *  this process optimizes the memory usage and reduces internal fragmentation
 *
 *  vsa: pointer to the initialized VSA (`vsa_t`) that requires defragmentation
 *
 *  returns: no return value 
 */
static void Defragmentation(vsa_t *vsa)
{
	header_t *current_header = NULL;	
	header_t *next_header = NULL;		
	
	assert(vsa);
	
	current_header = (header_t*)vsa->first_header;
	next_header = (header_t*)current_header->next_block;	
	
	while(current_header != NULL && next_header != NULL)		
	{
		if((current_header->is_free == 1) && (next_header->is_free == 1))
		{
			current_header->block_size += next_header->block_size;	
			current_header->next_block = next_header->next_block;
			next_header->cookie = 0;				
			next_header = next_header->next_block;
		}								
		else
		{
			current_header = current_header->next_block;			
			next_header = next_header->next_block;
		}
	}	
}

/*
 * Function:  ManageBlockRemainder 
 * --------------------
 *  manages the remainder of a memory block after an allocation in the VSA.
 *
 *  this function it divides the block into two parts - the first part is allocated for the requested size,
 *  and the second part, if large enough, is marked as free, creating a new available block
 * 
 *  current_header:      pointer to the header of the allocated memory block
 *  saved_next:          pointer to the next block's header 
 *  block_size:          size (in bytes) of the requested memory block, including the header
 *  original_block_size: original size (in bytes) of the free memory block before the allocation
 *
 *  returns: no return value
 */
static void ManageBlockRemainder(header_t *current_header, void *saved_next, size_t block_size, size_t original_block_size)
{
	if(original_block_size - block_size >= (sizeof(header_t) + WORD_SIZE)) 
	{
		current_header->next_block = (char*)current_header + block_size;	
		current_header->block_size = block_size; 				
		current_header = current_header->next_block;				
		
		current_header->is_free = 1;						
		current_header->block_size = original_block_size - block_size;	
		current_header->cookie = COOKIE;					
		
		if(saved_next == NULL)							
		{
			current_header->next_block = NULL;				
		} 
		else									
		{
			current_header->next_block = saved_next;			
		}			
	}
}


/*
 * Function:  VsaInit 
 * --------------------
 *  initializes a VSA for efficient memory management of variable-sized blocks
 *
 *  this function sets up an Allocator, starting from the provided memory address `alloc_dest`
 *  the allocator ensures that the memory address is properly aligned to the system's WORD size
 *
 *  alloc_dest:   starting memory address where the VSA will manage memory blocks
 *  size:	   total size (in bytes) of the memory region that the VSA will manage,
 *		   including space needed for the management structure and headers
 *
 *  returns: a pointer to the `vsa_t` structure, representing the initialized VSA
 *	     this pointer can be used for subsequent memory allocation and deallocation operations within the VSA
 */
vsa_t *VsaInit(void *alloc_dest, size_t size)
{
	vsa_t *my_vsa = NULL; 								
	header_t *header = NULL;                                      		
	size_t remainder_address = 0;							 
	char *start_address = (char*)alloc_dest;					
 	
	assert(alloc_dest);    				        		
	
	remainder_address = (size_t)alloc_dest % WORD_SIZE;
	
	/* align address to WORD size */
	if(0 != remainder_address) 							
	{
		size = size - (WORD_SIZE - remainder_address);
		start_address += (WORD_SIZE - remainder_address);
	}
	
	/* make sure 'size' has enough space for management struct & first header */
	assert(size >= (sizeof(vsa_t) + sizeof(header_t))); 				
	
	my_vsa = (vsa_t*)start_address;						
	my_vsa->first_header = (size_t*)start_address + sizeof(vsa_t)/WORD_SIZE;
	
	header = (header_t*)my_vsa->first_header;					
	header-> is_free = 1;			 		
	header->next_block = NULL;
	header->block_size = size - sizeof(vsa_t);					
	header->cookie = COOKIE;
	
	return my_vsa; 
}


/*
 * Function:  VsaFree 
 * --------------------
 *  takes a pointer to a memory block (`block`) in the VSA and marks it as free, making it available for future allocations
 *
 *  block: pointer to the memory block that needs to be freed
 *
 *  returns: no return value 
 */
void VsaFree(void *block)
{
	header_t *current_header = NULL;
	
	assert(block);									
	
	current_header = (header_t*)block - 1;							
	
	#ifdef DEBUG
		assert(current_header->cookie == COOKIE);
	#endif	
	
	current_header->is_free = 1;									
}


/*		
 * Function:  VsaLargestChunk 
 * --------------------
 *  returns the size of the largest available memory chunk (free) in the VSA by iterating through all the headers
 *  the function internally calls `Defragmentation` to optimize memory layout before locating the largest memory chunk
 *
 *  vsa: pointer to the initialized VSA to analyze
 *
 *  returns: size (in bytes) of the largest available memory chunk (minus header size) or 0 if no blocks are available
 */
size_t VsaLargestChunk(vsa_t *vsa)
{
	header_t *current_header = NULL;					
	size_t largest_chunk = 0;
	
	assert(vsa);
	
	current_header = (header_t*)vsa->first_header;
	
	Defragmentation(vsa);
	
	while(current_header != NULL)
	{
		if((current_header->is_free == 1) && (current_header->block_size > largest_chunk)) 
		{
			largest_chunk = current_header->block_size - sizeof(header_t);		
		}
		current_header = current_header->next_block;						
	}
	
	return largest_chunk;
}

		
/*		
 * Function:  VsaAlloc 
 * --------------------
 *  allocates a memory block of the specified size from the VSA
 *
 *  this function searches the VSA for a free memory block that can accommodate the requested size and marks the corresponding header as used
 *  if the found block is larger than needed, it is divided and by this creates a new available block
 *  the function internally calls `Defragmentation` to optimize memory layout before allocating the block
 *
 *  vsa:	   pointer to the initialized VSA to allocate from
 *  block_size:   the requested size (in bytes) of the memory block to allocate
 *
 *  returns: a pointer to the start of the allocated memory block, or NULL if unsuccessful
 */
void *VsaAlloc(vsa_t *vsa, size_t block_size)
{  
	void *saved_next = NULL; 
	void *return_block = NULL;
	size_t remainder_block = block_size % WORD_SIZE; 			
	size_t original_block_size = 0;
	header_t *current_header = NULL;
	
	assert(vsa);									
	
	current_header = (header_t*)vsa->first_header;
	original_block_size = current_header->block_size;
	
	/* change block size to WORD if needed & add header size */
	if(0 != remainder_block)   							
	{
		block_size += (WORD_SIZE - remainder_block);
	}
	block_size += sizeof(header_t); 						
	
	Defragmentation(vsa);
	
	while(NULL != current_header)											
	{
		original_block_size = current_header->block_size;
		
		if((current_header->is_free == 1) && (current_header->block_size >= block_size))	
		{
			saved_next = current_header->next_block;					 
			current_header->is_free = 0; 							
			current_header->cookie = COOKIE;						
			return_block = (size_t*)current_header + (sizeof(header_t)/WORD_SIZE);	
			
			ManageBlockRemainder(current_header, saved_next, block_size, original_block_size);	
			
			return return_block;								
		}
		current_header = current_header->next_block;						
	}
	return NULL;											
}
