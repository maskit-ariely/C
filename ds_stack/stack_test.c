#include "stack.h"
#include "utilities.h"

int main()
{
	int x = 5;
	int y = 10;
	void *ptr_x = &x;
	void *ptr_y = &y;	
	stack_t *ptr = NULL;
	
	
	/***** StackCreate *****/
	printf("\n\n----- StackCreate -----\n\n");
	ptr = StackCreate(2, 4);
	TESTS(0 == IsStackEmpty(ptr));
	TESTS(2 == StackCapacity(ptr));	
	
	/* pop element when stack is empty */
	TESTS(-1 == StackPop(ptr)); 
	
	
	/***** StackPush *****/
	printf("\n\n----- StackPush -----\n\n");
	TESTS(0 == StackPush(ptr, ptr_x)); 
	TESTS(5 == *(char*)StackPeek(ptr))
	TESTS(0 == StackPush(ptr, ptr_y)); 
	TESTS(10 == *(char*)StackPeek(ptr))
	
	TESTS(1 == IsStackEmpty(ptr));
	TESTS(2 == StackSize(ptr));
	
	/* push element when stack is full */
	TESTS(-1 == StackPush(ptr, ptr_x)); 


	/***** StackPop *****/
	printf("\n\n----- StackPop -----\n\n");
	TESTS(0 == StackPop(ptr)); 
	TESTS(1 == StackSize(ptr));
	TESTS(0 == StackPop(ptr)); 
	TESTS(0 == StackSize(ptr));
	TESTS(-1 == StackPop(ptr));


	StackDestroy(ptr);

	return 0;
}
