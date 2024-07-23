1 d8ex1: .PHONY d8ex1.o lib_calc1.so lib_loader.so lib_func.so
		gcc d8ex1.o -o d8ex1 -L. -l_calc1 -l_loader -lm
2 d8ex1.o: d8ex1.c main.h
		gcc d8ex1.c -c 

3 lib_calc1.so: Fcalc_dl1.o 
		gcc -shared Fcalc_dl1.o -o lib_calc1.so
4 Fcalc_dl1.o: Fcalc_dl1.c calclibs.h
		gcc -fPIC -c Fcalc_dl1.c

5 lib_loader.so: loader.o
		gcc -shared loader.o -o lib_loader.so
6 loader.o:     loader.c loader.h
		gcc -fPIC -c loader.c
		
7 lib_func.so: func.o
		gcc -shared func.o -o lib_func.so -lm

8 func.o:     func.c func.h
		gcc -fPIC -c func.c 	
		
9 .PHONY: clean.so clean.o

10 clean.so: clean.o
		rm -rf *.so
11 clean.o:
		rm -rf *.o
  


