1 all: .PHONY d13ex2_cl.o  dinamic_msg.o interface_cl.o
		gcc d13ex2_cl.o  dinamic_msg.o interface_cl.o -o d13ex2_cl -lpanel -lncurses

2 d13ex2_cl.o: d13ex2_cl.c 
		gcc d13ex2_cl.c -c 
		
		
3 .PHONY: clean.o

4 clean.o:
		rm -rf *.o

5 dinamic_msg.o: dinamic_msg.c dinamic_msg.h
		gcc dinamic_msg.c -c

6 interface_cl.o: interface_cl.c interface_cl.h
		gcc interface_cl.c -c

 
