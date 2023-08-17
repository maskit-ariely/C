
# Variable Sized Allocator (VSA) in C

This is a variable-sized allocator written in C. 
The allocator allows dynamic memory allocation and deallocation of variable-sized blocks.

## Features

- Dynamic memory allocation for variable-sized blocks.
- Efficient memory utilization and management.
- Defragmentation function to optimize memory layout and reduce fragmentation.
## Requirement

Make sure you have a C compiler installed on your machine (e.g., GCC).
## Compilation

A Makefile is included in the repository to simplify the compilation process. It utilizes `vsa_test.c` as the source of test cases to validate the code.<br>
To compile your C code and generate the executable, follow these steps:

1. Clone or download the repository to your local machine.
2. Open a terminal and navigate to the directory containing the downloaded files.
3. To compile the code, simply run the following command in the terminal:
   ```bash
   make
   ```
4. Execute the program with the following command:
   ```bash
   ./vsa.out
   ```
## Usage

To use the variable-sized allocator in your C project, follow these steps:
1. Initialize the allocator by calling VsaInit, passing the starting memory address (alloc_dest) and the total size (in bytes) of the memory region.
2. Use VsaAlloc to allocate variable-sized memory blocks from the VSA. 
3. Use VsaFree to free a previously allocated memory block.

## Known Issues

The VsaAlloc function is designed to allocate memory by selecting the first available block that is large enough to accomodate the requested allocation, rather than scanning the entirety of available blocks to find the optimal (i.e., smallest suitable) block of memory.
