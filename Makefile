serial:
	g++ -fopenmp lcs-serial.cpp -o lcs-serial
openmp: 
	g++ -fopenmp lcs-omp.cpp -o lcs-omp
mpi:
	mpic++ lcs-mpi.cpp -o lcs-mpi
openmpi:
	mpic++ -fopenmp lcs-mpi.cpp -o lcs-openmpi	
clean:
	rm -f lcs-omp lcs-serial lcs-mpi *.o

zip:
	zip  g12omp lcs-serial.cpp lcs-mpi.cpp
