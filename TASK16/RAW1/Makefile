CC = gcc
//compile_flags = -Werror -pedantic-errors -Wall -Wextra -Wpedantic
//targetS =      ./d16raw
targetC_udp =  ./d16udp_ncl
targetC_tcp  = ./d16udp_ns
 

COMPMAKE = make
REMOVE = rm -rf *.o $(targetS) $(targetC_udp) $(targetC_tcp)

all:  .PHONY *.o  $(targetS) $(targetC_udp) $(targetC_tcp)
	@echo Done!
	
$(targetS): $(targetS).o 
		$(CC) $(targetS).o -o $(targetS)

$(targetC_tcp): $(targetC_tcp).o 
		$(CC) $(targetC_tcp).o -o $(targetC_tcp)
		
$(targetC_udp): $(targetC_udp).o 
		$(CC) $(targetC_udp).o -o $(targetC_udp)


*.o: *.c
		$(CC) $(compile_flags) -c *.c

.PHONY: clean.o
	$(REMOVE)
