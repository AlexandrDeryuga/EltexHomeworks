1 all: d13ex1serv_SV.c clSV posix_serv posix_cl
		gcc d13ex1serv_SV.c -o d13ex1serv_SV -lrt
2 clSV: d13ex1cl_SV.c
		gcc d13ex1cl_SV.c -o d13ex1cl_SV
3 posix_serv: d13ex1serv_posix.c
		gcc d13ex1serv_posix.c -o d13ex1serv_posix -lrt
4 posix_cl: d13ex1cl_posix.c
		gcc d13ex1cl_posix.c -o d13ex1cl -lrt
		



