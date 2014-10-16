all:
	g++ -o lcs lcs-test.cpp

omp:
	g++ -fopenmp -o lcs-omp lcs-test.cpp

clean:
	rm -f *.o lcs lcs-omp
