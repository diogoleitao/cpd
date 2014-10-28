all:
	g++ -fopenmp -o lcs-omp lcs.cpp

clean:
	rm -f lcs-omp *.cpp~ Makefile~
