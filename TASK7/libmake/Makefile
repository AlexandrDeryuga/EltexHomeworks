1 d7ex1: d7ex1.o Fcalc_1_add.o Fcalc_2_min.o Fcalc_3_mult.o Fcalc_4_div.o calc.a
		gcc d7ex1.o Fcalc_1_add.o Fcalc_2_min.o Fcalc_3_mult.o Fcalc_4_div.o  -o d7ex1
2 d7ex1.o: d7ex1.c main.h
		gcc d7ex1.c -c 
3 calc.a: Fcalc_1_add.o Fcalc_2_min.o Fcalc_3_mult.o Fcalc_4_div.o
		ar rc calc.a Fcalc_1_add.o Fcalc_2_min.o Fcalc_3_mult.o Fcalc_4_div.o
4 Fcalc_1_add.o: Fcalc_1_add.c calclibs.h 
		gcc Fcalc_1_add.c -c 
5 Fcalc_2_min.o: Fcalc_2_min.c calclibs.h 
		gcc Fcalc_2_min.c -c 
6 Fcalc_3_mult.o: Fcalc_3_mult.c calclibs.h 
		gcc Fcalc_3_mult.c -c 
7 Fcalc_4_div.o: Fcalc_4_div.c calclibs.h 
		gcc Fcalc_4_div.c -c 




