#include <stdio.h>
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
int** TABLE;
int** TRACKER;


/***********************
 ** AUXILIARY PROCEDURES
 ***********************/

//Prints the auxiliary table
void printTable(){
	for(int i = 0; i <= N_LENGTH; i++){
		for(int j = 0; j <= M_LENGTH; j++)
			printf(" %d |", TABLE[i][j]);
		printf("\n");
	}
	printf("\n\n");
}

//Reads an input file with a given name and initializes global variables
int initProblem(string filename)
{
	FILE * myfile;
	myfile = fopen(filename.c_str(), "r");
	
	if (myfile != NULL){
		//Get the size of the strings
		fscanf(myfile, "%d %d", &N_LENGTH, &M_LENGTH);
		
		//Create the arrays to store the strings;
		N = new char[N_LENGTH + 1];
		M = new char[M_LENGTH + 1];

		//Get the strings
		fscanf(myfile, "%s", N);
		fscanf(myfile, "%s", M);

		//Close file
		fclose(myfile);

		//Create auxiliary tables
		TABLE = new int*[N_LENGTH+1];
		TRACKER = new int*[N_LENGTH+1];
		
		for(int i = 0; i < N_LENGTH+1; ++i){
			TABLE[i] = new int[M_LENGTH+1]();
			TRACKER[i] = new int[M_LENGTH+1]();
		}
	}
	else{
		printf("Could not open input file (named %s).", filename.c_str());
		return 0;
	}

	return 1;
}

//Computes the solution using an auxiliary table
void computeSolution()
{
	for(int i = 1; i <= N_LENGTH; i++){
		for(int j = 1; j <= M_LENGTH; j++){
			if(N[i-1] == M[j-1]){
				TABLE[i][j] = TABLE[i-1][j-1] + 1;
				//DIAGONAL - IT'S A MATCH
				TRACKER[i][j] = MATCH;
			} else if(TABLE[i-1][j] > TABLE[i][j-1]) {
				TABLE[i][j] = TABLE[i-1][j];
				//UP - FETCH VALUE FROM PREVIOUS SUBPROBLEM
				TRACKER[i][j] = UP;
			} else { TABLE[i][j] = TABLE[i][j-1];
				//LEFT - FETCH VALUE FROM CURRENT SUBPROBLEM
				TRACKER[i][j] = LEFT;
			}
		}
	}
}

//Prints the result of our previous computation
void printResult()
{
	int length = TABLE[N_LENGTH][M_LENGTH];
	char result[length+1];
	result[length--] = '\0';
	
	//Prints the size of the biggest subsequence
	printf("%d\n", TABLE[N_LENGTH][M_LENGTH]);
	
	//Tracks the biggest subsequence
	int i=N_LENGTH;
	int j=M_LENGTH; 
	
	while(length >= 0){
		if(TRACKER[i][j] == MATCH){
			result[length--] = N[i-1];
			--i;
			--j;
		}
		else if(TRACKER[i][j] == UP) --i;
		else --j;
	}
	
	printf("%s\n", result);
}


/***********************
 ** MAIN
 ***********************/

int main(int argc, const char* argv[])
{

	//The problem has one input. Another file.
	if(argc != 2){
		printf("lcs-serial should have *1* input, it has *%d*.", argc-1);
		return 0;
	}
	
	//Initialize
	if(!initProblem(argv[1]))
		printf("lcs-serial failed to initialize the problem.");
	
	//Compute
	computeSolution();
	
	//Print
	printResult();
	
	return 0;
}
