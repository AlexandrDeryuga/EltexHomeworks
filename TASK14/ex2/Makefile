1 all: .PHONY d13ex2.o dinamic_msg.o interface.o rtest.o
		gcc d13ex2.o dinamic_msg.o interface.o rtest.o -o d13ex2 -lpanel -lncurses

2 d13ex2.o: d13ex2.c 
		gcc d13ex2.c -c 
3 rtest.o: rtest.c msg_by_sm.h 
		gcc rtest.c -c 
		
		
4 .PHONY: clean.o

5 clean.o:
		rm -rf *.o

6 dinamic_msg.o: dinamic_msg.c dinamic_msg.h
		gcc dinamic_msg.c -c

7 interface.o: interface.c interface.h
		gcc interface.c -c

 
