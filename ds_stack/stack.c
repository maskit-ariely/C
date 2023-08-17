#include "stack.h"
#include <stdlib.h>	/* malloc */
#include <assert.h>	/* assert */
#include <string.h>	/* memcpy */

/*
 * Struct:  stack 
 * --------------------
 *  defines a dynamic stack data structure
 * 
 *  base:	pointer to the beginning of the stack's elements 
 *  top:	pointer to the location above the most recently added element
 *      	when the stack is empty, `top` is equal to `base`.
 *  capacity:	the total number of elements that the stack can hold
 *  elem_size:	the size of each element in bytes.
 */
struct stack
{
	void *base;
	void *top;
	size_t capacity;
	size_t elem_size;
};


/*		
 * Function:  StackCreate 
 * --------------------
 *  allocates memory for a new stack of a specified capacity and element size
 *
 *  this function attempts to allocate memory for both the stack structure
 *  and the array that will hold the stack's elements
 *  if either allocation fails, the function cleans up any previously allocated memory and returns NULL
 *
 *  capacity:    number of elements to be stored in the stack
 *  elem_size:   size (in bytes) of each element
 *
 *  returns: 	  a pointer to the newly created stack, or NULL if memory allocation failed
 */
stack_t *StackCreate(const size_t capacity,const size_t elem_size)
{
	stack_t *stack = NULL;

	if(capacity == 0 || elem_size == 0)
	{
		return NULL;
	}
	
	stack = malloc(sizeof(stack_t));
	if(NULL == stack)
	{
		return NULL;
	}
	
	stack->base = malloc(capacity * elem_size);
	if(NULL == stack->base)
	{
		free(stack);
		return NULL;
	}
	
	stack->elem_size = elem_size;
	stack->capacity = capacity;
	stack->top = stack->base;

	return stack;
}

 
/*		
 * Function:  StackDestroy
 * --------------------
 *  deallocates memory of a stack that was previously created
 *  by freeing the stack's elements and the stack structure itself
 *
 *  stack:   pointer to the stack to be destroyed
 *
 *  returns: no return value
 */ 
void StackDestroy(stack_t *stack)
{
	assert(stack);
	
	free(stack->base);
	free(stack);
}


/*		
 * Function:  StackPush
 * --------------------
 *  pushes a new element onto the top of the stack
 *
 *  this function copies the element pointed to by `elem` to the current top of the stack
 *  after the element is copied, the stack's top is updated to the new position
 *
 *  stack:  pointer to the stack
 *  elem:   pointer to the element to be pushed onto the stack
 *
 *  returns: 0 for success, or -1 for failure (stack is full)
 */
int StackPush(stack_t *stack ,const void *elem)
{
	assert(stack);
	
	if(StackCapacity(stack) == StackSize(stack))
	{
		return -1;
	}
	
	memcpy(stack->top, elem, stack->elem_size);
	stack->top = (char*)stack->top + stack->elem_size;
	
	return 0;
}


/*		
 * Function:  StackPeek
 * --------------------
 *  retrieves the top element of the stack without removing it
 *
 *  stack:	pointer to the stack
 *
 *  returns:	a pointer to the top element of the stack or NULL if the stack is empty
 */
void *StackPeek(const stack_t *stack)
{
	assert(stack);
	
	if(0 == IsStackEmpty(stack))
	{
		return NULL;
	}
	
	return (char*)stack->top - stack->elem_size;
}


/*		
 * Function:  StackPop
 * --------------------
 *  removes the top element from the stack
 *
 *  this function decreases the stack's top pointer by the size of one element, 
 *  effectively removing the top element from the stack
 *  the actual memory is not deallocated or cleaned, but is now considered available
 *
 *  stack:	pointer to the stack
 *
 *  returns:	0 for success, or -1 for failure (stack is empty)
 */
int StackPop(stack_t *stack)
{
	assert(stack);
	
	if(0 == IsStackEmpty(stack))
	{
		return -1;
	}
	
	stack->top = (char*)stack->top - stack->elem_size;
	return 0;
}


/*		
 * Function:  IsStackEmpty
 * --------------------
 *  checks if the stack is empty
 *
 *  this function checks if the top of the stack has retreated to its base, 
 *  which indicates that the stack is empty.
 *
 *  stack:	pointer to the stack
 *
 *  returns:	0 if the stack is empty, 1 if the stack contains one or more elements
 */
int IsStackEmpty(const stack_t *stack)
{
	assert(stack);
	
	return (stack->top <= stack->base ? 0 : 1); 
}


/*		
 * Function:  StackSize
 * --------------------
 *  computes the current number of elements in the stack
 *
 *  his function calculates the difference between the top of the stack and the base, 
 *  divided by the size of each element. 
 *
 *  stack:	pointer to the stack
 *
 *  returns:	the current number of elements in the stack
 */
size_t StackSize(const stack_t *stack)
{
	assert(stack);
	
	return ((char*)stack->top - (char*)stack->base) / stack->elem_size;
}


/*		
 * Function:  StackCapacity
 * --------------------
 *  retrieves the total capacity of the stack
 *
 *  stack:	pointer to the stack
 *
 *  returns:	total capacity of the stack
 */
size_t StackCapacity(const stack_t *stack)
{	
	assert(stack);
	
	return stack->capacity;
}
