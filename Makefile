all:
	g++ -fopenmp lcs-omp.cpp -o lcs-omp
	g++ -fopenmp lcs-serial.cpp -o lcs-serial
clean:
	rm -f lcs-omp lcs-serial

zip:
	zip  g12omp lcs-serial.cpp lcs-omp.cpp
