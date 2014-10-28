all:
	g++ -fopenmp lcs-omp.cpp -o lcs-omp

zip:
	zip  g12omp lcs-serial.cpp lcs-omp.cpp
