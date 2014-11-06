all:	serial omp opt
serial:
	gcc -g lcs-serial.c -lm -o lcs-serial
omp:
	gcc -g lcs-omp.c -lm -o lcs-omp
opt:
	gcc -g lcs-omp-opt.c -lm -o opt

