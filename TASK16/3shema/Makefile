CC = gcc
//compile_flags = -Werror -pedantic-errors -Wall -Wextra -Wpedantic
targetS_ntcp = ./d16tcp_ns
targetC_ntcp = ./d16tcp_ncl


COMPMAKE = make
REMOVE = rm -rf *.o $(targetS_ntcp) $(targetC_ntcp) 

all:  .PHONY *.o  $(targetS_ntcp) $(targetC_ntcp) 
	@echo Done!
	
$(targetS_ntcp): $(targetS_ntcp).o
		$(CC) $(targetS_ntcp).o -o $(targetS_ntcp)

$(targetC_ntcp): $(targetC_ntcp).c 
		$(CC) $(targetC_ntcp).o -o $(targetC_ntcp)
	
*.o: *.c
		$(CC) $(compile_flags) -c *.c

.PHONY: clean.o
	$(REMOVE)
