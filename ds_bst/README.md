
# Binary Search Tree (BST) implementation in C


## Features

- Implements common BST operations: insertion, deletion, and search
- Supports standard traversal methods: in-order, pre-order, and post-order
- Handles duplicate values by placing them to the right of the original node
- Provides a functionality to execute a user-defined function on each node
## Requirement

Make sure you have a C compiler installed on your machine (e.g., GCC).
## Compilation

A Makefile is included in the repository to simplify the compilation process.<br>
To compile your C code and generate the executable, follow these steps:

1. Clone or download the repository to your local machine.
2. Open a terminal and navigate to the directory containing the downloaded files.
3. To compile the code, simply run the following command in the terminal:
   ```bash
   make
   ```
4. Execute the program with the following command:
   ```bash
   ./bst.out
   ```

## Usage

Essential BST operations:
 
- BstCreate: Initializes the BST with a comparison function.
- BstInsert: Adds a new node with the specified data.
- BstRemove: Removes a node with the given data.
- BstFind: Searches for a node containing the specified data.

Utility Functions:
- BstHeight: Retrieves the height of the BST.
- BstSize: Fetches the current number of nodes in the BST.
- BstIsEmpty: Determines if the BST is empty.

Advanced:
- BstForEach: Traverses the BST in specified order (in-order, pre-order, post-order) and performs a user-defined function on each node.