/*
 *   CPD
 *
 *   MPI - LCS Implementation
 *  
 */

#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

/***********************
 ** GLOBAL VARIABLES
 ***********************/
//MPI_VARIABLES
MPI_Status status;

//Directions
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

int pos[2];
int p;
int id;


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
void divideInput(int p) {
	int i, j, k;

	char* new_N; // odd N
	char* new_M; // temporary vector
	int new_M_LENGTH = 0;
	int new_N_LENGTH = 0;
	char* tmp;

	//Divide input
	//1. P is odd; Considering N as lines and M as columns. Review if necessary;
	if ((p % 2) != 0) {
		//If it's odd, we can only divide columns
		//N goes whole; M is divided into processors.
		tmp = M;
		new_M_LENGTH = floor(M_LENGTH / p); //each processor will have a part of M
		M_LENGTH -= (p-1) * new_M_LENGTH; //correct last M if it's bigger: Total = first M - (p-1)floored M

		for (i = 1; i <= p-1; i++) {
			k = 0;
			for (j = 0; j < new_M_LENGTH; j++) {
				new_M[j] = M[k];
				k++;
			}

			MPI_Send(&N_LENGTH, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&new_M_LENGTH, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(N, N_LENGTH + 1, MPI_CHAR, i, 2, MPI_COMM_WORLD);
			MPI_Send(new_M, new_M_LENGTH + 1, MPI_CHAR, i, 3, MPI_COMM_WORLD);
		}

		for (j = 0; j < M_LENGTH; j++) {
			new_M[j] = M[k];
			k++;
		}

		//Sends last block. Separated because it may be of different size
		MPI_Send(&N_LENGTH, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		MPI_Send(&M_LENGTH, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
		MPI_Send(N, N_LENGTH + 1, MPI_CHAR, i, 2, MPI_COMM_WORLD);
		MPI_Send(new_M, M_LENGTH + 1, MPI_CHAR, i, 3, MPI_COMM_WORLD);
	} else { // p is even; Divide M into p parts, divide N into 2 parts 
		new_N_LENGTH = floor(N_LENGTH / 2); // LENGTH FOR N, in top part of matrix.
		new_M_LENGTH = p / 2; // NAO ESTAVA A SER INICIALIZADO
		k = 0;
		for (i = 0; i < new_N_LENGTH; i++) {
			new_N[i] = N[k]; //New_N is top part of the Matrix; N is bottom;
			k++;
		}

		//TOP MATRIX:
		int limit = p / 2;
		int l = 0;
		for (i = 1; i <= limit-1; i++) {
			for (j = 0; j < new_M_LENGTH; j++) {
				new_M[j] = M[l];
				l++;
			}

			MPI_Send(&new_N_LENGTH, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&new_M_LENGTH, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(new_N, new_N_LENGTH + 1, MPI_CHAR, i, 2, MPI_COMM_WORLD);
			MPI_Send(new_M, new_M_LENGTH + 1, MPI_CHAR, i, 3, MPI_COMM_WORLD);
		}

		for (j = 0; j < M_LENGTH; j++) {
			new_M[j] = M[l];
			l++;
		}

		//Sends last block. Separated because it may be of different size
		MPI_Send(&new_N_LENGTH, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD);
		MPI_Send(&M_LENGTH, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
		MPI_Send(new_N, new_N_LENGTH + 1, MPI_CHAR, i+1, 2, MPI_COMM_WORLD);
		MPI_Send(new_M, M_LENGTH + 1, MPI_CHAR, i+1, 3, MPI_COMM_WORLD);

		//BOTTOM MATRIX
		N_LENGTH -= new_N_LENGTH;
		for (i = 0; i < N_LENGTH; i++) {
			new_N[i] = N[k]; //New_N is now bottom part of the Matrix;
			k++;
		}

		l = 0;
		for (i = 1; i <= limit-1; i++) {
			for (j = 0; j < new_M_LENGTH; j++) {
				new_M[j] = tmp[l];
				l++;
			}

			MPI_Send(&N_LENGTH, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&new_M_LENGTH, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			MPI_Send(new_N, N_LENGTH + 1, MPI_CHAR, i, 2, MPI_COMM_WORLD);
			MPI_Send(new_M, new_M_LENGTH + 1, MPI_CHAR, i, 3, MPI_COMM_WORLD);
		}

		for (j = 0; j < M_LENGTH; j++) {
			new_M[j] = tmp[l];
			l++;
		}

		//Sends last block. Separated because it may be different size
		MPI_Send(&N_LENGTH, 1, MPI_INT, i+1, 0, MPI_COMM_WORLD);
		MPI_Send(&M_LENGTH, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
		MPI_Send(N, N_LENGTH + 1, MPI_CHAR, i+1, 2, MPI_COMM_WORLD);
		MPI_Send(new_M, M_LENGTH + 1, MPI_CHAR, i+1, 3, MPI_COMM_WORLD);
	}
}

void receiveInput(int p, int id) {
	MPI_Recv(&N_LENGTH, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(&M_LENGTH, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

	//Create the arrays to store the strings;
	N = new char[N_LENGTH + 1];
	M = new char[M_LENGTH + 1];

	MPI_Recv(N, N_LENGTH + 1, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);
	MPI_Recv(M, M_LENGTH + 1, MPI_CHAR, 0, 3, MPI_COMM_WORLD, &status);
}

//Reads an input file with a given name and initializes global variables
void initProblem() {
	int i = 0;
	int correctedSizeN = N_LENGTH + 1;
	int correctedSizeM = M_LENGTH + 1;

	//Create auxiliary tables
	TABLE = new short*[correctedSizeN];
	TRACKER = new short*[correctedSizeN];

	for (i = 0; i < N_LENGTH + 1; i++) {
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
void printResult(int l, int c) {
	int length = TABLE[N_LENGTH][M_LENGTH], i, j;
	char result[length + 1];
	result[length--] = '\0';

	//Prints the size of the biggest subsequence
	//cout << TABLE[N_LENGTH][M_LENGTH] << endl  -> NOT APPLIED ANYMORE!;
	pos[2] = 0;
	//Tracks the biggest subsequence
	if (l == 0) {
		i = N_LENGTH;
		j = c;
	} else {
		i = l;
		j = M_LENGTH;
	}
	// ALGORITMO TEM QUE SER ALTERADO; ATÉ: ATINGIR VALOR ZERO.
	while (TABLE[i][j] != 0) {
		if (TRACKER[i][j] == MATCH) {
			result[length--] = N[i-1];
			--i;
			--j;
		} else if (TRACKER[i][j] == UP)
			--i;
		else
			--j;
	}

	if ((p % 2) == 0) {
		int left_pos, top_pos;
		if (id > ((p/2) + 1)) { // FOR ALL BOTTOM MATRIXES
			if (j == 0){ // GO LEFT
				pos[0] = i;
				id--;
				MPI_Send(&id, 1, MPI_INT, id-1, 5, MPI_COMM_WORLD);
				MPI_Send(&pos, 2, MPI_INT, id-1, 4, MPI_COMM_WORLD);  
			}

			if (i == 0) { // GO TOP
				pos[1] = j; 
				id -= (p/2);
				MPI_Send(&id, 1, MPI_INT, id-(p/2), 5, MPI_COMM_WORLD);
				MPI_Send(&pos, 2, MPI_INT, id-(p/2), 4, MPI_COMM_WORLD);  
			} else {
				MPI_Recv(&left_pos, 1, MPI_INT, id-1, 7, MPI_COMM_WORLD, &status); //Receive j position from id-a
				MPI_Recv(&top_pos, 1, MPI_INT, id - (p/2), 7, MPI_COMM_WORLD, &status); //Receive i position from id-(p/2)
				if (left_pos >= top_pos) { // GO LEFT
					pos[0] = i; 
					id--;
					MPI_Send(&id, 1, MPI_INT, id-1, 5, MPI_COMM_WORLD);
					MPI_Send(&pos, 2, MPI_INT, id-1, 4, MPI_COMM_WORLD);  
				} else { // GO TOP
					pos[1] = j; 
					id -= (p/2);
					MPI_Send(&id, 1, MPI_INT, id-(p/2), 5, MPI_COMM_WORLD);
					MPI_Send(&pos, 2, MPI_INT, id-(p/2), 4, MPI_COMM_WORLD);  
				}
			}
			MPI_Send(&result, strlen(result), MPI_INT, 0, 8, MPI_COMM_WORLD); // STRING TO id = 0
		} else if (id == (p/2) + 1) {
				if (j == 0) { 
					MPI_Send(&result, strlen(result), MPI_INT, 0, 8, MPI_COMM_WORLD);
					MPI_Send(&UP,1,MPI_INT,0, 6, MPI_COMM_WORLD); // SET TO TERMINATE
				} else { // GO TOP
					pos[1] = j;
					id -= (p/2);
					MPI_Send(&id, 1, MPI_INT, id-(p/2), 5, MPI_COMM_WORLD);
					MPI_Send(&pos, 2, MPI_INT, id-(p/2), 4, MPI_COMM_WORLD);  
				}
			} else { // IS ON TOP
				if (i == 0) {
					MPI_Send(&result, strlen(result), MPI_INT, 0, 8, MPI_COMM_WORLD);
					MPI_Send(&UP,1,MPI_INT,0, 6, MPI_COMM_WORLD); // SET TO TERMINATE, o UP é só para ter &1
				} else { // GO LEFT
					pos[0] = i; 
					id--;
					MPI_Send(&id, 1, MPI_INT, id-1, 5, MPI_COMM_WORLD);
					MPI_Send(&pos, 2, MPI_INT, id-1, 4, MPI_COMM_WORLD);  
				}
			}
			MPI_Send(&result, strlen(result), MPI_INT, 0, 8, MPI_COMM_WORLD); // MPI_Send string to id=0
	} else {
		pos[0] = i; 
		id--;
		MPI_Send(&id, 1, MPI_INT, id-1, 5, MPI_COMM_WORLD);
		MPI_Send(&result, strlen(result), MPI_INT, 0, 8, MPI_COMM_WORLD); // String to main
	}
}

/***********************
 ** MAIN
 ***********************/
int main (int argc, char *argv[]) {
	int i, j;
	char* result;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

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
	} else { // INDIVIDUAL PROCESSES:
		receiveInput(p, id);
		initProblem();
		computeSolution();
	}

	if (id == p)  // Bottom last matrix
		printResult(0, 0);
	else {
		int current_id, initial_value;
		while (id != 0) {
			if (id > (p/2)) {
				MPI_Recv(pos, 2, MPI_INT, MPI_ANY_SOURCE, 7, MPI_COMM_WORLD, &status);
				i = pos[0];
				j = pos[1];
				if (i == 0)
					MPI_Send(&TABLE[i][j], 1, MPI_INT, id+(p/2), 7, MPI_COMM_WORLD);
				else
					MPI_Send(&TABLE[i][j], 1, MPI_INT, id+1, 7, MPI_COMM_WORLD);

			MPI_Recv(&current_id, 1, MPI_INT, MPI_ANY_SOURCE, 5, MPI_COMM_WORLD, &status);
			MPI_Recv(&initial_value, 2, MPI_INT, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, &status); //RECEIVE POS
			
			if (id == current_id); // COMPLETE
				//printResult(initial_value[0], initial_value[1]); WHY ERROR?
			}
		// ADD BARRIER?
		}
	}

	if (id == 0) {
		int terminate = 0, string_size = 0;
		char *str;
		//REVIEW!
		while (terminate == 0) {
			//MPI_Recv() // RECEIVE SIZE OF STRING
			//str = malloc(string_size+1);
			//MPI_Recv() // RECEIVE STRING
			//result = realloc(strlen(result)+string_size); // REVIEW LIMITS FOR SEGFAULT
			strcat(result, str);
			// RECEIVE STRINGS AND CONCATENATE
			MPI_Recv(&terminate, 1, MPI_INT, MPI_ANY_SOURCE, 6, MPI_COMM_WORLD, &status);
		}
	}

	cout << strlen(result) << endl;
	cout << result << endl;

	MPI_Finalize();
	return 0;
}
