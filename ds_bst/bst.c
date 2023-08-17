#include <stdio.h>	/* printf */
#include <stdlib.h>	/* malloc */
#include <assert.h>
#include "bst.h"

/*		
 * Struct:  bst_node 
 * --------------------
 *  represents a node in the BST
 *
 *  data:   a pointer to the data stored in the current node (any type data)
 *  right:  a pointer to the right child node of the current node
 *  left:   a pointer to the left child node of the current node
 */
struct bst_node
{
	void *data;
	struct bst_node *right;
	struct bst_node *left;
};

/*		
 * Struct:  bst 
 * --------------------
 *  represents the management structure for the BST
 *
 *  root:            a pointer to the root node of the BST
 *  compare_func_t:  a function pointer to the comparison function used 
 *                   to determine the ordering of elements in the BST 
 *                   the function should return:
 *                   - a negative integer if (first < second)
 *                   - zero if the two elements are equal
 *                   - a positive integer if (first > second)
 */
struct bst
{
	struct bst_node *root;
	int (*compare_func_t)(const void*, const void*);
};


/*		
 * Function:  BstCreate 
 * --------------------
 *  initializes a new BST management structure 
 *
 *  cmp_func:    a pointer to the comparison function which determines the 
 *               ordering of elements in the BST
 *
 *  returns: 	  a pointer to the newly created bst_t structure, or NULL if memory allocation fails
 */
bst_t *BstCreate(compare_func_t cmp_func)
{
	bst_t *management = NULL;
	
	assert(cmp_func);
	
	management = malloc(sizeof(bst_t));		
	if(NULL == management)					
	{
		return NULL;
	}
	
	management->root = NULL;
	management->compare_func_t = cmp_func;
	
	return management;
}


/*		
 * Function:  InsertHelper 
 * --------------------
 *  an internal helper function that recursively inserts a new node with the specified data 
 *  into the BST according to the ordering determined by the comparison function
 *
 *  node:       a pointer to the current node in the BST
 *  compare:    a pointer to the comparison function used to determine the 
 *              ordering of elements in the BST
 *  data:       the data that should be stored in the new node
 *
 *  returns: 	 a pointer to the (possibly new) node at the current position 
 *              in the BST, or NULL if memory allocation for the new node failed
 */
static struct bst_node* InsertHelper(struct bst_node *node, compare_func_t compare, const void *data)
{
	struct bst_node *new_node = node;

	if(NULL == new_node)
	{
		/* create new node */
		new_node = malloc(sizeof(struct bst_node));		
		if(NULL == new_node)
		{
			return NULL;
		}
		
		new_node->data = (void*)data;
		new_node->right = NULL;
		new_node->left = NULL; 
		
		return new_node;
	}
	
	/* traverse through the right subtree (data is bigger) */
	if(compare(data, node->data) > 0)					
	{
		new_node = InsertHelper(new_node->right, compare, data);
		node->right = new_node; 
		
	}
	/* traverse through the left subtree (data is smaller) */
	else
	{
		new_node = InsertHelper(new_node->left, compare, data);		
		node->left = new_node; 
	}
	
	return node;
}


/*		
 * Function:  BstInsert 
 * --------------------
 *  inserts a new node with the specified data into the provided BST 
 *  
 *  this function is the public interface and uses the internal helper function 'InsertHelper' 
 *  the root of the BST may change during the insertion, so the root node is reassigned back 
 *
 *  bst:       a pointer to the BST management structure
 *  data:      the data that should be stored in the new node
 *
 *  returns:   0 if the insertion was successful, or 1 if the insertion failed 
 */
int BstInsert(bst_t *bst, const void *data)
{	
	assert(bst);
	assert(data);

	bst->root = InsertHelper(bst->root, bst->compare_func_t, data);		

	if(NULL == bst->root)																	
	{
		return 1;
	}
	
	return 0;
}

   
/*		
 * Function:  BstIsEmpty 
 * --------------------
 *  checks if the BST provided is empty by checking if the root of the tree is NULL
 *
 *  bst:       a pointer to the BST management structure
 *
 *  returns:   1 if the BST is empty, or 0 if the BST is not empty
 */
int BstIsEmpty(const bst_t *bst)
{
	assert(bst);

	return (NULL == bst->root? 1 : 0);			
}


/*		
 * Function:  InOrder 
 * --------------------
 *  an internal helper function that performs an in-order traversal of the BST provided
 *  left subtree -> root node -> right subtree 
 *  
 *  node:          a pointer to the current node in the BST
 *  action_func:   a pointer to the action function to be called for each visited node 
 *                 this function should return 0 for success or non-zero for failure
 *  param:         a pointer to an additional parameter that will be passed to the action function
 *
 *  returns:       0 if the entire BST was traversed successfully, or 1 if the traversal was terminated early
 */
static int InOrder(struct bst_node *node, action_function_t action_func, void *param)
{
	if(NULL == node)
	{
		return 0;
	}

	/* left subtree -> current node */
	if(InOrder(node->left, action_func, param) || action_func(node->data, param))			
	{
		return 1;
	}

	/* right subtree */
	return InOrder(node->right, action_func, param);
}


/*		
 * Function:  PreOrder 
 * --------------------
 *  an internal helper function that performs a pre-order traversal of the BST provided
 *  root node -> left subtree -> right subtree 
 *
 *  node:          a pointer to the current node in the BST
 *  action_func:   a pointer to the action function to be called for each visited node 
 *                 this function should return 0 for success or non-zero for failure
 *  param:         a pointer to an additional parameter that will be passed to the action function
 *
 *  returns:       0 if the entire BST was traversed successfully, or 1 if the traversal was terminated early
 */
static int PreOrder(struct bst_node *node, action_function_t action_func, void *param)
{
	if(NULL == node)
	{
		return 0;
	}

	/* current node -> left subthree */
	if(action_func(node->data, param) || PreOrder(node->left, action_func, param))
	{
		return 1;
	}		

	/* right subtree */
	return PreOrder(node->right, action_func, param);	
}


/*		
 * Function:  PostOrder 
 * --------------------
 *  an internal helper function that performs a post-order traversal of the BST provided
 *  left subtree -> right subtree -> root node 
 *
 *  node:          a pointer to the current node in the BST
 *  action_func:   a pointer to the action function to be called for each visited node 
 *                 this function should return 0 for success or non-zero for failure
 *  param:         a pointer to an additional parameter that will be passed to the action function
 *
 *  returns:       0 if the entire BST was traversed successfully, or 1 if the traversal was terminated early
 */
static int PostOrder(struct bst_node *node, action_function_t action_func, void *param)
{
	if(NULL == node)
	{
		return 0;
	}

	/* left subtree -> right subtree */
	if(PostOrder(node->left, action_func, param) || PostOrder(node->right, action_func, param))		
	{
		return 1;
	}

	/* current node */
	return action_func(node->data, param);													
}


/*		
 * Function:  ForEac 
 * --------------------
 *  traverses the BST provided according to the specified traversal mode
 *
 *  bst:          a pointer to the BST management structure
 *  mode:         the traversal mode (IN_ORDER, PRE_ORDER, or POST_ORDER)
 *  action_func:  a pointer to the action function to be called for each visited node 
 *                this function should return 0 for success or non-zero for failure
 *  param:        a pointer to an additional parameter that will be passed to the action function
 *
 *  returns:      0 if the entire BST was traversed successfully, or 1 if the traversal was terminated early
 *                returns -1 if an invalid traversal mode is specified
 */
int BstForEach(bst_t *bst, traversal_t mode, action_function_t action_func, void *param)
{
	int return_value = -1;

	assert(bst);
	assert(action_func);

	switch(mode)
	{
		case IN_ORDER:
			return_value = InOrder(bst->root, action_func, param);
			break;

		case PRE_ORDER:
			return_value = PreOrder(bst->root, action_func, param);
			break;

		case POST_ORDER:
			return_value = PostOrder(bst->root, action_func, param);
			break;
	}

	return return_value;
}


/*		
 * Function:  SizeHelper 
 * --------------------
 *  an internal helper function intended to increment the count of nodes during a BST traversal
 *
 *  data:       a pointer to the data of the currently visited node (not utilized in current implementation)
 *  parameter:  a pointer to a variable that accumulates the count of nodes
 *
 *  returns:    always returns 0, indicating that the traversal should continue 
 */
static int SizeHelper(void *data, void *parameter)
{
	++*(size_t*)(parameter);
	(void)data;
	
	return 0;
}


/*		
 * Function:  BstSize 
 * --------------------
 *  computes and returns the total number of nodes in the BST provided
 *
 *  bst:      a pointer to the BST management structure
 *
 *  returns:  total number of nodes in the BST
 */
size_t BstSize(const bst_t *bst)
{
	size_t sum = 0;

	assert(bst);
	
	InOrder(bst->root, SizeHelper, &sum);		

	return sum;
}


/*		
 * Function:  DestroyHelper 
 * --------------------
 *  an internal helper function that traverses the BST provided to recursively free all nodes
 *
 *  this function ensures safe deallocation by first freeing the left and right 
 *  subtrees of the current node, and then freeing the node itself
 *
 *  node:      a pointer to the current node in the BST
 *
 *  returns:	no return value
 */
static void DestroyHelper(struct bst_node *node)	
{
	if(NULL == node)
	{
		return;
	}
	
	DestroyHelper(node->left);
	DestroyHelper(node->right);
	free(node);
}


/*		
 * Function:  BstDestroy 
 * --------------------
 *  deallocates all memory associated with the BST provided,
 *  including all nodes and the management structure itself
 *
 *  bst:      a pointer to the BST management structure
 *
 *  returns: no return value
 */
void BstDestroy(bst_t *bst)
{
	assert(bst);
	
	DestroyHelper(bst->root);

	free(bst);							
}


/*		
 * Function:  FindHelper 
 * --------------------
 *  an internal helper function that performs a search for a specific data value within 
 *  the BST provided using the provided comparison function
 *
 *  node:      a pointer to the current node in the BST.
 *  data:      a pointer to the target data value being searched for
 *  cmp_func:  a pointer to the comparison function used to compare node data values
 *
 *  returns:   the matching data if found; otherwise, NULL
 */
static void *FindHelper(struct bst_node *node, const void *data, compare_func_t cmp_func)
{
	int cmp_result = 0;
	
	if(NULL == node)
	{
		return NULL;
	}

	cmp_result = cmp_func(node->data, data);
	
	/* match found */
	if(0 == cmp_result)				
	{
		return node->data;
	}

	/* data is smaller than node's data */
	if (cmp_result > 0)
   	{
        	return FindHelper(node->left, data, cmp_func);
    	}
	
	/* data is larger than node's data */
	return FindHelper(node->right, data, cmp_func);
}


/*		
 * Function:  BstFind 
 * --------------------
 *  searches the BST provided for a node containing the specified data
 *
 *  bst:	a pointer to the BST management structure
 *  data:	a pointer to the target data value being searched for
 *
 *  returns:	the matching data if found within the BST; otherwise, NULL.
 */
void *BstFind(bst_t *bst, const void *data)
{
	assert(bst);
	assert(data);

	return FindHelper(bst->root, data, bst->compare_func_t);
}


/*		
 * Function:  HeightHelper 
 * --------------------
 *  an internal helper function that recursively calculates the height of the BST provided
 *
 *  node:      a pointer to the current node in the BST
 *
 *  returns:   the height of the taller subtree of the two child subtrees
 */
size_t HeightHelper(struct bst_node *node)
{
	size_t left_height = 0;
	size_t right_height = 0;

	if (NULL == node)
	{
		return 0;
	}
    	else 
	{
	/* traverse through the tree */
        left_height = HeightHelper(node->left);			
        right_height = HeightHelper(node->right);
        
        /* increment when returning from recursion */
	return (left_height >= right_height) ? left_height + 1 : right_height + 1;		
    }
}


/*		
 * Function:  BstHeight 
 * --------------------
 *  computes the height of the BST provided, which is defined as the
 *  number of edges on the longest path from the root node to a leaf node
 *
 *  bst:      a pointer to the BST management structure
 *
 *  returns:  the height of the BST
 */
size_t BstHeight(const bst_t *bst)
{
	assert(bst);

	return HeightHelper(bst->root);
}


/*		
 * Function:  GetMaxValNode 
 * --------------------
 *  an internal helper function traverses the BST through the right
 *  and finds the node with the maximum data value 
 *
 *  node:      a pointer to the current node in the BST
 *
 *  returns:   a pointer to the node with the maximum data value 
 */
static struct bst_node* GetMaxValNode(struct bst_node* node)
{
    struct bst_node* current = node;

    while (current && current->right != NULL)		
	{
		current = current->right;
	}
 
    return current;
}


/*		
 * Function:  RemoveHelper 
 * --------------------
 *  an internal helper function that removes the node with the specified data from the BST provided
 *  if there are multiple occurrences, it removes the first one
 *
 *  node:       a pointer to the current node in the BST
 *  cmp_func:   a pointer to the comparison function used to compare node data values
 *  data:       a pointer to the target data value to be removed
 *
 *  returns:    a pointer to the (possibly new) node at the current position 
 */
static struct bst_node* RemoveHelper(struct bst_node *node, compare_func_t cmp_func, void *data)
{
	struct bst_node* temp = NULL;
	int compare_return = cmp_func(data, node->data);

	if(NULL == node)
	{
		return NULL;
	}

	/* traverse the tree through the left */
	if(-1 == compare_return)										
	{
		node->left = RemoveHelper(node->left, cmp_func, data);
	}

	/* traverse the tree through the right */
	else if(1 == compare_return)									
	{
		node->right = RemoveHelper(node->right, cmp_func, data);
	}

	else
	{	
		/* if received data is found save it and send it back through the function */
		if(node->left == NULL) 										
		{
            		temp = node->right;
            		free(node);
		    	return temp;
		}
		else if(node->right == NULL) 
		{
		    	temp = node->left;
			free(node);
	 		return temp;
		}

		temp = GetMaxValNode(node->left);					/* traverse the tree to find maximum value on the subtree found */
		node->data = temp->data;						/* update data from node found */
		node->left = RemoveHelper(node->left, cmp_func, temp->data);		/* keep traversing the tree through the left and delete the node being found */
	}

	return node;
}


/*		
 * Function:  BstRemove 
 * --------------------
 *  removes the node with the specified data from the BST provided 
 *  after removal, the BST structure is maintained and the root might be adjusted.
 *
 *  bst:   a pointer to the BST management structure
 *  data:  a pointer to the target data value to be removed
 *
 * returns: no return value
 */
void BstRemove(bst_t *bst, void *data)
{
	bst->root = RemoveHelper(bst->root, bst->compare_func_t, data);	
}
