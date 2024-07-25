1 all: .PHONY d9ex2.o scandir.o
		gcc d9ex2.o scandir.o -o d9ex2 -lpanel -lncurses

2 d9ex2.o: d9ex2.c 
		gcc d9ex2.c -c 
		
3 scandir.o: scandir.c scandir.h
		gcc scandir.c -c
		
4 .PHONY: clean.o

5 clean.o:
		rm -rf *.o



