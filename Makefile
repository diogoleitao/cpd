all:
	g++ -fopenmp lcs-omp.cpp -o lcs-omp
	g++ -fopenmp lcs-serial.cpp -o lcs-serial
	mpic++ lcs-mpi.cpp -o lcs-mpi
	mpic++ teste-mpi.cpp -o teste
clean:
	rm -f lcs-omp lcs-serial lcs-mpi *.o

zip:
	zip  g12omp lcs-serial.cpp lcs-omp.cpp lcs-mpi.cpp
