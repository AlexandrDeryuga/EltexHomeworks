1 All: d14serv_sv.o d14cl_sv d14cl_pos d14s_pos
		gcc d14serv_sv.o -o d14serv_sv
2 d14cl_sv: d14cl_sv.o 
		gcc d14cl_sv.o -o d14cl_sv

3 d14serv_sv.o: d14serv_sv.c message.h
		gcc d14serv_sv.c -c
4 d14cl_sv.o:   d14cl_sv.c message.h 
		gcc d14cl_sv.c -c 
		
		
5 .PHONY: clean.o 

6 clean.o:
		rm -rf *.o
		
7 d14cl_pos:  d14cl_pos.c
		gcc d14cl_pos.c -o d14cl_pos
8 d14s_pos: d14s_pos.c
		gcc d14s_pos.c -o d14s_pos
		
		
		
		





