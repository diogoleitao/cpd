/*
 *   CPD
 *
 *   MPI - LCS Implementation
 *  
 */

#include <mpi.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;

/***********************
 ** GLOBAL VARIABLES
 ***********************/

//Direccions
int MATCH = 0;
int UP = 1;
int LEFT = 2;
 
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
		dcost += pow(sin((double) x), 2) + pow(cos((double) x), 2);

	return (short) (dcost / n_iter + 0.1);
}

//Divides and sends ; Needs reviewing; Remove code redundancy
void divideInput(int p){
	int i, j, k;

	char* new_N; // odd N
	char* new_M; // temporary vector
	int new_M_LENGTH = 0;
	int new_N_LENGTH = 0;
	char* tmp;

	//Divide input
	//1. P is pair; Considering N as lines and M as columns. Review if necessary;
	if ((p % 2) != 0) {
		//If it's odd, we can only divide columns
		//N Goes complete; M is divided into processors.
		tmp = M;
		new_M_lenght = floor(M/p); //each processor will have a part of M
		M_LENGTH -= (p-1)*new_M_lenght; //correct last M if it's bigger: Total = first M - (p-1)floored M

		for(i=1; i <= (p-1); i++){
			for(j=0; j < new_M_lenght; j++){
				new_M[j] = M[j];
			}

			MPI_Send(&N_LENGTH, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&new_M_lenght, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(N, N_LENGTH + 1, MPI_CHAR, i, 2, MPI_COMM_WORLD);
			MPI_Send(new_M, new_M_lenght + 1, MPI_CHAR, i, 3, MPI_COMM_WORLD);

			M.erase(0, j);
		}
			for(j=0; j < M_lenght; j++){
				new_M[j] = M[j];
			}
			//Sends last block. Separated because it may be different size
			MPI_Send(&N_LENGTH, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&M_lenght, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(N, N_LENGTH + 1, MPI_CHAR, i, 2, MPI_COMM_WORLD);
			MPI_Send(new_M, M_lenght + 1, MPI_CHAR, i, 3, MPI_COMM_WORLD);
	}

	else{ // p is pair; Divide M into p parts, divide N into 2 parts 
			new_N_LENGTH = floor (N_LENGTH / 2); // LENGTH FOR N, in top part of matrix.
			for(i=0; i<new_N_LENGTH; i++){
				new_N[i] = N[i]; //New_N is top part of the Matrix; N is bottom;
			}
			N.erase(0, new_N_LENGTH);

			//TOP MATRIX:
			int limit = p/2;

			for(i=1; i <= (limit-1); i++){
				for(j=0; j < new_M_lenght; j++){
					new_M[j] = M[j];
				}

				MPI_Send(&new_N_LENGTH, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&new_M_lenght, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
				MPI_Send(new_N, new_N_LENGTH + 1, MPI_CHAR, i, 2, MPI_COMM_WORLD);
				MPI_Send(new_M, new_M_lenght + 1, MPI_CHAR, i, 3, MPI_COMM_WORLD);

				M.erase(0, j);
			}
			for(j=0; j < M_lenght; j++){
				new_M[j] = M[j];
			}
			//Sends last block. Separated because it may be different size
			MPI_Send(&new_N_LENGTH, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD);
			MPI_Send(&M_lenght, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
			MPI_Send(new_N, new_N_LENGTH + 1, MPI_CHAR, i+1, 2, MPI_COMM_WORLD);
			MPI_Send(new_M, M_lenght + 1, MPI_CHAR, i+1, 3, MPI_COMM_WORLD);

			//BOTTOM MATRIX
			N_LENGTH -= new_N_LENGTH;

			for(i=1; i <= (limit-1); i++){
				for(j=0; j < new_M_lenght; j++){
					new_M[j] = tmp[j];
				}

				MPI_Send(&N_LENGTH, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&new_M_lenght, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
				MPI_Send(N, N_LENGTH + 1, MPI_CHAR, i, 2, MPI_COMM_WORLD);
				MPI_Send(new_M, new_M_lenght + 1, MPI_CHAR, i, 3, MPI_COMM_WORLD);

				tmp.erase(0, j);
			}
			for(j=0; j < M_lenght; j++){
				new_M[j] = tmp[j];
			}
			//Sends last block. Separated because it may be different size
			MPI_Send(&N_LENGTH, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD);
			MPI_Send(&M_lenght, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
			MPI_Send(N, N_LENGTH + 1, MPI_CHAR, i+1, 2, MPI_COMM_WORLD);
			MPI_Send(new_M, M_lenght + 1, MPI_CHAR, i+1, 3, MPI_COMM_WORLD);

	}
}

//Reads an input file with a given name and initializes global variables
void initProblem() {
	int i = 0;
	int correctedSizeN = N_LENGTH + 1;
	int correctedSizeM = M_LENGTH + 1;

	//Create auxiliary tables
	TABLE = new short*[correctedSizeN];
	TRACKER = new short*[correctedSizeN];

	for (i = 0; i < N_LENGTH + 1; ++i) {
		TABLE[i] = new short[correctedSizeM]();
		TRACKER[i] = new short[correctedSizeM]();
	}
}

//Computes the solution using an auxiliary table
void computeSolution() {
	int i = 0, j = 0;
	short n, m;

	for (i = 1; i <= N_LENGTH; i++) {
		for (j = 1; j <= M_LENGTH; j++) {
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
	}
}

//Prints the result of our previous computation
void printResult() {
	int length = TABLE[N_LENGTH][M_LENGTH];
	char result[length + 1];
	result[length--] = '\0';

	//Prints the size of the biggest subsequence
	cout << TABLE[N_LENGTH][M_LENGTH] << endl;

	//Tracks the biggest subsequence
	int i = N_LENGTH;
	int j = M_LENGTH;

	while (length >= 0) {
		if (TRACKER[i][j] == MATCH) {
			result[length--] = N[i-1];
			--i;
			--j;
		} else if (TRACKER[i][j] == UP)
			--i;
		else
			--j;
	}

	cout << result << endl;
}


/***********************
 ** MAIN
 ***********************/

int main (int argc, char *argv[]) {

	MPI_Status status;
	int id, p, i;
	double secs;

	p = argv[1];
	MPI_Init (&argc, &argv);

	MPI_Comm_rank (MPI_COMM_WORLD, &id);
	//Shouldn't p be argv[1] ? Where do we pass the number of processes?
	MPI_Comm_size (MPI_COMM_WORLD, &p);
  
	if (!id) {
		FILE *myfile;
		string filename = argv[1];
		myfile = fopen(filename.c_str(), "r");

		if (myfile != NULL) {
			if (fscanf(myfile, "%d %d", &N_LENGTH, &M_LENGTH) <= 0)
				exit(1);

			//Create the arrays to store the strings;
			N = new char[N_LENGTH + 1];
			M = new char[M_LENGTH + 1];

			if (fscanf(myfile, "%s", N) <= 0)
				exit(1);
			if (fscanf(myfile, "%s", M) <= 0)
				exit(1);

			// divides and sends messages
			divideInput(p);

			
		} else {
			printf ("%s: input file not found.", filename.c_str());
			MPI_Finalize();
			exit(1);
		}

	} else {
		MPI_Recv(&N_LENGTH, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&M_LENGTH, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
	
		//Create the arrays to store the strings;
		N = new char[N_LENGTH + 1];
		M = new char[M_LENGTH + 1];

		MPI_Recv(N, N_LENGTH + 1, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);
		MPI_Recv(M, M_LENGTH + 1, MPI_CHAR, 0, 3, MPI_COMM_WORLD, &status);
	}

	//Initialize	
	initProblem();

	//Compute
    computeSolution();

	//Print
	printResult();
 
	//MPI_Barrier (MPI_COMM_WORLD);

    /*if (id == p-1) {
		//Faz coisas
    } else {
		//Comunica

		//Faz coisas
	}*/

    MPI_Finalize();
    return 0;
}

