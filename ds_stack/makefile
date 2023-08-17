CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra 
STACK_SOURCE = stack.h stack.c stack_test.c

##############################################################################

stack: $(SOURCES)
	@$(CC) $(CFLAGS) $(STACK_SOURCE) -o stack.out
	
.o_stack: $(SOURCES)
	@$(CC) $(CFLAGS) $(STACK_SOURCE) -c

debug_stack: $(SOURCES)
	@$(CC) $(CFLAGS) -g $(STACK_SOURCE) -o stack_debug.out

