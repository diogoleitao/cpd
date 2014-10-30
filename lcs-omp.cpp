#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <omp.h>
#include <math.h>

using namespace std;

/***********************
 ** GLOBAL VARIABLES
 ***********************/

//Direccions
#define MATCH 0
#define UP 1
#define LEFT 2

//Input strings
char* N;
char* M;

//String size
int N_LENGTH;
int M_LENGTH;

//Auxiliary tables
short** TABLE;
short** TRACKER;


/***********************
 ** AUXILIARY PROCEDURES
 ***********************/

//The cost routine
short cost(int x) {
	int i, n_iter = 20;
	double dcost = 0;
	
	for (i = 0; i < n_iter; i++)
		dcost += pow(sin((double) x),2) + pow(cos((double) x),2);

	return (short) (dcost / n_iter + 0.1);
}

//Prints the auxiliary table
void printTable() {
	for (int i = 0; i <= N_LENGTH; i++) {
		for (int j = 0; j <= M_LENGTH; j++)
			printf(" %d |", TABLE[i][j]);
		printf("\n");
	}
	printf("\n\n");
}

//Reads an input file with a given name and initializes global variables
void initProblem() {
	int i = 0;
	int correctedSizeN = N_LENGTH + 1;
	int correctedSizeM = M_LENGTH + 1;

	//Create auxiliary tables
	TABLE = new short*[correctedSizeN];
	TRACKER = new short*[correctedSizeN];

#pragma omp parallel for
	for (i = 0; i < N_LENGTH + 1; ++i) {
		TABLE[i] = new short[correctedSizeM]();
		TRACKER[i] = new short[correctedSizeM]();
	}
}

//Computes the value of a single position of the table;
void computePosition(int i, int j) {
	if (N[i-1] == M[j-1]) {
		TABLE[i][j] = TABLE[i-1][j-1] + cost(j);
		//DIAGONAL - IT'S A MATCH
		TRACKER[i][j] = MATCH;
	} else if (TABLE[i-1][j] > TABLE[i][j-1]) {
		TABLE[i][j] = TABLE[i-1][j];
		//UP - FETCH VALUE FROM PREVIOUS SUBPROBLEM
		TRACKER[i][j] = UP;
	} else {
		TABLE[i][j] = TABLE[i][j-1];
		//LEFT - FETCH VALUE FROM CURRENT SUBPROBLEM
		TRACKER[i][j] = LEFT;
	}
}

//Computes the solution using an auxiliary table
void computeSolution() {
	int imin = 1, imax = 1, jmin = 1, jmax = 1;
	int diagonal, k;
	int minDim = (N_LENGTH<M_LENGTH)?N_LENGTH:M_LENGTH;
	int dimDiff = abs(N_LENGTH - M_LENGTH);

	//Phase 1: increasing workload
	for(diagonal = 1; imax < minDim; imax++, jmax++, diagonal++){
#pragma omp parallel for schedule(dynamic,64)
		for(k = 0; k < diagonal; k++){
			computePosition(imax-k, jmin+k);
		}
	}

	//Phase 2: constant workload
	for(int iter = 0; iter < dimDiff; iter++, (imax < N_LENGTH)?imax++:jmin++, (jmax < M_LENGTH)?jmax++:imin++){
#pragma omp parallel for
		for(k = 0; k < diagonal; k++){
			computePosition(imax-k, jmin+k);
		}
	}

	//Phase 3: decreasing workload
	for(; imin <= imax; imin++, jmin++, diagonal--){
#pragma omp parallel for schedule(guided,64)
		for(k = 0; k < diagonal; k++){
			computePosition(imax-k, jmin+k);
		}
	}
}

//Prints the result of our previous computation
void printResult() {
	int length = TABLE[N_LENGTH][M_LENGTH];
	char result[length + 1];

	//Prints the size of the biggest subsequence
	cout << TABLE[N_LENGTH][M_LENGTH] << endl;;

	result[length--] = '\0';

	//Tracks the biggest subsequence
	int i = N_LENGTH;
	int j = M_LENGTH;
	
	while (length >= 0) {
		if (TRACKER[i][j] == MATCH) {
			result[length--] = N[i-1];
			--i;
			--j;
		}
		else if(TRACKER[i][j] == UP)
			--i;
		else
			--j;
	}

	cout << result << endl;
}


/***********************
 ** MAIN
 ***********************/

int main(int argc, const char* argv[]) {
	double start = omp_get_wtime();	
	
	FILE *myfile;
	string filename = argv[1];
	myfile = fopen(filename.c_str(), "r");
	
	if (myfile != NULL){

		fscanf(myfile, "%d %d", &N_LENGTH, &M_LENGTH);

		//Create the arrays to store the strings;
		N = new char[N_LENGTH + 1];
		M = new char[M_LENGTH + 1];

		fscanf(myfile, "%s", N);
		fscanf(myfile, "%s", M);
		
		initProblem();

		//Compute
		computeSolution();
		
		//Print
		printResult();
		
		

	}
	double end = omp_get_wtime(), time = end - start;
	cout << time << endl;

	return 0;
}
