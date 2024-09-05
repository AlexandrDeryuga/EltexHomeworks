CC = gcc
//compile_flags = -Werror -pedantic-errors -Wall -Wextra -Wpedantic
targetS_broad = ./d16broad_s
targetC_broad = ./d16broad_cl
targetS_mult  = ./d16mult_s
targetC_mult  = ./d16mult_cl
 

COMPMAKE = make
REMOVE = rm -rf *.o $(targetS_broad) $(targetC_broad) $(targetS_mult) $(targetC_mult) 

all:  .PHONY *.o  $(targetS_broad) $(targetC_broad) $(targetS_mult) $(targetC_mult)
	@echo Done!
	
$(targetS_broad): $(targetS_broad).o
		$(CC) $(targetS_broad).o -o $(targetS_broad)

$(targetC_broad): $(targetC_broad).c 
		$(CC) $(targetC_broad).o -o $(targetC_broad)
		
$(targetS_mult): $(targetS_mult).o
		$(CC) $(targetS_mult).o -o $(targetS_mult)

$(targetC_mult): $(targetC_mult).c 
		$(CC) $(targetC_mult).o -o $(targetC_mult)
	
*.o: *.c
		$(CC) $(compile_flags) -c *.c

.PHONY: clean.o
	$(REMOVE)
