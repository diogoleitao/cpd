all:
	g++ -fopenmp -o lcs-omp lcs.cpp

clean:
	rm -f lcs-omp *.cpp~ Makefile~

zip:
	zip  g12omp lcs-serial.cpp lcs-omp.cpp
