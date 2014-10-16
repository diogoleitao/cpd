#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

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

//Computes the solution using an auxiliary table
void computeSolution() {
	int i = 0, j = 0;
	short n, m;

	//#pragma omp parallel for private(j)

	// nao funciona em paralelo
	for (i = 1; i <= N_LENGTH; i++) {
		for (j = 1; j <= M_LENGTH; j++) {
			if (N[i-1] == M[j-1]) {
				TABLE[i][j] = TABLE[i-1][j-1] + 1;
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
	cout << TABLE[N_LENGTH][M_LENGTH] << endl;;

	//Tracks the biggest subsequence
	int i = N_LENGTH;
	int j = M_LENGTH;
	int k = 0;

	//#pragma omp parallel for
	// nao funciona em paralelo
	for (k = length; k >= 0; k--) {
		if (TRACKER[i][j] == MATCH) {
			result[length--] = N[i-1];
			--i;
			--j;
		}
		else if(TRACKER[i][j] == UP) {
			--i;
			++k;
		}
		else {
			--j;
			++k;
		}
	}

	/*while (length >= 0) {
		if (TRACKER[i][j] == MATCH) {
			result[length--] = N[i-1];
			--i;
			--j;
		}
		else if(TRACKER[i][j] == UP)
			--i;
		else
			--j;}*/

	cout << result << endl;
}


/***********************
 ** MAIN
 ***********************/

int main(int argc, const char* argv[]) {
	ios::sync_with_stdio(false);
	cin.tie(0);
	cin >> N_LENGTH >> M_LENGTH;

	//Create the arrays to store the strings;
	N = new char[N_LENGTH + 1];
	M = new char[M_LENGTH + 1];

	cin >> N;
	cin >> M;

	initProblem();

	//Compute
	computeSolution();
	
	//Print
	printResult();
	
	return 0;
}
