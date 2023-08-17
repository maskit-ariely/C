CC = gcc
CFLAGS = -ansi -pedantic-errors -Wall -Wextra 
VSA_SOURCE = vsa.c vsa_test.c vsa.h

##############################################################################

# description: compile files
vsa: $(SOURCES)
	@$(CC) $(CFLAGS) $(VSA_SOURCE)  -o vsa.out

# description: compile without linking
.o_vsa: $(SOURCES)
	@$(CC) $(CFLAGS) $(VSA_SOURCE) -c

# description: remove files with specific endings
clean_vsa: 
	@rm *.o *.out

# description: do the following actions	
all_vsa: debug .o make 

# description: compile with debug
debug_vsa: $(SOURCES)
	@$(CC) $(CFLAGS) -g -DDEBUG $(VSA_SOURCE) -o vsa_debug.out
	
# description: compile with optimization
release_vsa: $(SOURCES)
	@$(CC) $(CFLAGS) $(VSA_SOURCE) -O3 -o vsa_release.out

