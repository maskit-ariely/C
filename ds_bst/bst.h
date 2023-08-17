#ifndef BST_H
#define BST_H

#include <stddef.h>	/* size_t */

typedef enum {IN_ORDER, PRE_ORDER, POST_ORDER} traversal_t;

typedef struct bst bst_t;

/* a pointer to the action function to be called for each visited node */
typedef int (*action_function_t)(void *data, void *parameter);

/* a function pointer to the comparison function used to determine the ordering of elements */
typedef int (*compare_func_t)(const void *data1, const void *data2);


/* initializes a new BST management structure  */
bst_t *BstCreate(compare_func_t cmp_func);

/* deallocates all memory associated with the BST provided */
void BstDestroy(bst_t *bst);

/* removes the node with the specified data from the BST provided */
void BstRemove(bst_t *bst, void *data);


/* inserts a new node with the specified data into the provided BST */
int BstInsert(bst_t *bst, const void *data);

/* computes the height of the BST provided */
size_t BstHeight(const bst_t *bst);

/* computes and returns the total number of nodes in the BST provided */
size_t BstSize(const bst_t *bst);

/* checks if the BST provided is empty by checking if the root of the tree is NULL */
int BstIsEmpty(const bst_t *bst);

/* searches the BST provided for a node containing the specified data */
void *BstFind(bst_t *bst, const void *data);

/* traverses the BST provided according to the specified traversal mode */
int BstForEach(bst_t *bst, traversal_t mode, action_function_t action_func, void *param);


#endif /* BST_H*/
