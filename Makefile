all:	serial omp opt
serial:
	gcc -g -fopenmp lcs-serial.c -lm -o lcs-serial
omp:
	gcc -g -fopenmp lcs-omp.c -lm -o lcs-omp
opt:
	gcc -g -fopenmp lcs-omp-opt.c -lm -o opt
clean:
	rm lcs-serial lcs-omp opt
