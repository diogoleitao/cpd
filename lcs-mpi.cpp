/*
 *   CPD
 *
 *   MPI - LCS Implementation
 *  
 */

#include <stdio.h>
#include <string.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <mpi.h>

using namespace std;

/***********************
 ** GLOBAL VARIABLES
 ***********************/

//Direccions
#define MATCH 0
#define UP 1
#define LEFT 2
#define MAXWIDTH 128

//MPI variables
	MPI_Status status;
  int id, p;

//Input strings
char* N;
char* M;

//String size
int N_LENGTH;
int M_LENGTH;

//Auxiliary tables
short** TABLE;


/***********************
 ** AUXILIARY PROCEDURES
 ***********************/

//Prints the auxiliary table
void printTable() {
	for (int i = 0; i < N_LENGTH; i++) {
		for (int j = 0; j < M_LENGTH; j++)
			printf(" %d |", TABLE[i][j]);
		printf("\n");
	}
	printf("\n\n");
}

//The cost routine
short cost(int x) {
	int i, n_iter = 20;
	double dcost = 0;
	
	for (i = 0; i < n_iter; i++)
		dcost += pow(sin((double) x), 2) + pow(cos((double) x), 2);

	return (short) (dcost / n_iter + 0.1);
}

//Reads the input file
int readFile(string filename){		
	FILE *myfile;
	myfile = fopen(filename.c_str(), "r");

	if (myfile != NULL){
		if (fscanf(myfile, "%d %d", &N_LENGTH, &M_LENGTH) <= 0) {
			return 0;
		}

		//Create the arrays to store the strings;
		N = new char[N_LENGTH+1];
		M = new char[M_LENGTH+1];

		if (fscanf(myfile, "%s", N) <= 0) {
			return 0;
		}
		if (fscanf(myfile, "%s", M) <= 0) {
			return 0;
		}

	} else {
		return 0;
	}

	return 1;
}

void splitLines(){
	int rl = N_LENGTH; //remaining lines to split
	int rp = p; //remaining processes
	
	//split to process 0
	int index = 0;
	int size = floor(rl/rp);
	N_LENGTH = size;
	rl -= size;
	rp--;

	//split to other processes
	for(int i = 1; i < p; i++){
		index += size;
		size = floor(rl/rp);
		MPI_Send(&size, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
		MPI_Send(&M_LENGTH, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
		MPI_Send(&(N[index]), size, MPI_CHAR, i, 3, MPI_COMM_WORLD);		
		MPI_Send(M, M_LENGTH, MPI_CHAR, i, 4, MPI_COMM_WORLD);
		rl -= size;
		rp--;
	}	
}

//Initializes the auxiliary table
void initTable(int nlines, int ncols){
	TABLE = new short*[nlines];
	for (int i = 0; i < nlines; TABLE[i++] = new short[ncols]());
}

void computeBlock(int index, int size) {
	int i,j;
	int limit = index+size;

	for (i = 1; i < N_LENGTH; i++) {
		for (j = index; j < limit; j++) {
			if (N[i-1] == M[j-1])
				TABLE[i][j] = TABLE[i-1][j-1] + cost(j);
			else if (TABLE[i-1][j] > TABLE[i][j-1])
				TABLE[i][j] = TABLE[i-1][j];
			else 
				TABLE[i][j] = TABLE[i][j-1];
		}
	}
}

void printSolution(){
	int i = N_LENGTH-1;
	int j;	
	int length;
	int index;
	if(id == p-1){
		j = M_LENGTH-1;
		length = TABLE[i][j];
		index = length;
	} else{
		MPI_Recv(&length, 1, MPI_INT, id+1, 6, MPI_COMM_WORLD, &status);
		MPI_Recv(&index, 1, MPI_INT, id+1, 7, MPI_COMM_WORLD, &status);
		MPI_Recv(&j, 1, MPI_INT, id+1, 8, MPI_COMM_WORLD, &status);
	}
	
	char result[length + 1];
	
	if(id == p-1) result[index--] = '\0';
	else MPI_Recv(&(result[index+1]), length-index, MPI_CHAR, id+1, 9, MPI_COMM_WORLD, &status);
	
	while(i > 0 && j > 0){
		if(N[i-1] == M[j-1]){
			result[index--] = N[i-1];
			--i;
			--j;
		}
		else if(TABLE[i-1][j] > TABLE[i][j-1])
			--i;
		else
			--j;
	}
	
	if(id == 0){
		cout << length << endl;;
		cout << result << endl;
	} else {
		MPI_Send(&length, 1, MPI_INT, id-1, 6, MPI_COMM_WORLD);
		MPI_Send(&index, 1, MPI_INT, id-1, 7, MPI_COMM_WORLD);
		MPI_Send(&j, 1, MPI_INT, id-1, 8, MPI_COMM_WORLD);
		MPI_Send(&(result[index+1]), length-index, MPI_CHAR, id-1, 9, MPI_COMM_WORLD);
	}	
}

/***********************
 ** MAIN
 ***********************/
int main (int argc, char *argv[]) {

	//initialize MPI
	MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &id);
  MPI_Comm_size (MPI_COMM_WORLD, &p);
	if(id == 0){
		string filename = argv[1];		
		if(readFile(filename)) {
			splitLines();					
		} else {
			MPI_Finalize();
			return 0;
		}
	} else {
		MPI_Recv(&N_LENGTH, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&M_LENGTH, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
		N = new char[N_LENGTH+1];
		M = new char[M_LENGTH+1];
		MPI_Recv(N, N_LENGTH, MPI_CHAR, 0, 3, MPI_COMM_WORLD, &status);
		MPI_Recv(M, M_LENGTH, MPI_CHAR, 0, 4, MPI_COMM_WORLD, &status);
	}

	N_LENGTH++; M_LENGTH++;
	initTable(N_LENGTH, M_LENGTH);

	//Compute Solution
	int size;
	for(int j=1; j < M_LENGTH; j += MAXWIDTH){		 
		size = min(MAXWIDTH, M_LENGTH-j);
		if(id != 0) MPI_Recv(&(TABLE[0][j]), size, MPI_SHORT, id-1, 5, MPI_COMM_WORLD, &status);
		computeBlock(j, size);
		if(id != p-1) MPI_Send(&(TABLE[N_LENGTH-1][j]), size, MPI_SHORT, id+1, 5, MPI_COMM_WORLD);
	}

	//Print
	printSolution();

	MPI_Finalize();
	return 0;
}
