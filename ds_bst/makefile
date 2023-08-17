CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra 
BST_SOURCE = bst.h bst.c bst_test.c


##############################################################################

bst: $(SOURCES)
	@$(CC) $(CFLAGS) $(BST_SOURCE)  -o bst.out

debug_bst: $(SOURCES)
	@$(CC) $(CFLAGS) -g -DDEBUG $(BST_SOURCE) -o bst_debug.out

