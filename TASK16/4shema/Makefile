CC = gcc
//compile_flags = -Werror -pedantic-errors -Wall -Wextra -Wpedantic
targetS =      ./d16epoll
targetC_udp =  ./d16cl_udp
targetC_tcp  = ./d16cl_tcp
 

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
