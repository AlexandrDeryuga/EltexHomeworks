CC = gcc
compile_flags = -Werror -pedantic-errors -Wall -Wextra -Wpedantic
targetS_ltcp = ./d16tcp_ls
targetC_ltcp = ./d16tcp_lcl
targetS_ludp = ./d16udp_ls
targetC_ludp = ./d16udp_lcl
targetS_ltcp = ./d16tcp_ns
targetC_ltcp = ./d16tcp_ncl
targetS_ludp = ./d16udp_ns
targetC_ludp = ./d16udp_ncl


COMPMAKE = make
REMOVE = rm -rf *.o $(targetS_ltcp) $(targetC_ltcp) $(targetS_ludp) $(targetC_ludp) $(targetS_ntcp) $(targetC_ntcp) $(targetS_nudp) $(targetC_nudp)

all: *.o $(targetS_ltcp) $(targetC_ltcp) $(targetS_ludp) $(targetC_ludp) $(targetS_ntcp) $(targetC_ntcp) $(targetS_nudp) $(targetC_nudp)
	@echo Done!

$(targetS_ltcp): $(targetS_ltcp).o
	$(CC) $(targetS_ltcp).o -o $(targetS_ltcp)

$(targetC_ltcp): $(targetC_ltcp).o 
	$(CC) $(targetC_ltcp).o -o $(targetC_ltcp)
	
$(targetS_ludp): $(targetS_ludp).o
	$(CC) $(targetS_ludp).o -o $(targetS_ludp)

$(targetC_ludp): $(targetC_ludp).o 
	$(CC) $(targetC_ludp).o -o $(targetC_ludp)
	
$(targetS_ntcp): $(targetS_ntcp).o
	$(CC) $(targetS_ntcp).o -o $(targetS_ntcp)

$(targetC_ntcp): $(targetC_ntcp).o 
	$(CC) $(targetC_ntcp).o -o $(targetC_ntcp)
	
$(targetS_nudp): $(targetS_nudp).o
	$(CC) $(targetS_nudp).o -o $(targetS_nudp)

$(targetC_nudp): $(targetC_nudp).o 
	$(CC) $(targetC_nudp).o -o $(targetC_nudp)


*.o: *.c
	$(CC) $(compile_flags) -c *.c

clear:
	$(REMOVE)
