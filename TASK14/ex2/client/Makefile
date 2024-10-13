1 all: .PHONY d13ex2_cl.o  dinamic_msg.o interface_cl.o rtest.o
		gcc d13ex2_cl.o  dinamic_msg.o interface_cl.o rtest.o -o d13ex2_cl -lpanel -lncurses

2 d13ex2_cl.o: d13ex2_cl.c 
		gcc d13ex2_cl.c -c 
		
3 rtest.o: rtest.c msg_by_sm.h 
		gcc rtest.c -c 
		
		
		
4 .PHONY: clean.o

5 clean.o:
		rm -rf *.o

6 dinamic_msg.o: dinamic_msg.c dinamic_msg.h
		gcc dinamic_msg.c -c

7 interface_cl.o: interface_cl.c interface_cl.h
		gcc interface_cl.c -c

 
