void computeResult(int j, int id){
 
	int i = N_LENGTH; //LAST POSITION   - ASSUMING THIS VARIABLES ARE UPDATED FOR EACH PROCESS!
	string result;

	while(i != 0){
			if(N[i] == M[j]){ //MATCH
				result.insert(0, N[i]); //insert into the first pos of string, the letter
				i--;
				j--;
			}
			else if(TABLE[i][j-1] => TABLE [i-1][j]) // GO LEFT
				j--;
			else
				i--;
		}

	if (id != 0){
		int size = result.size();
		MPI_Send(&size, 1, MPI_INT, 0, 5, MPI_COMM_WORLD); // REVIEW A TAG
		MPI_Send(result, size, MPI_CHAR, 0, 6, MPI_COMM_WORLD ); // STRINGS DON'T NEED /0!; REVIEW TAG
		MPI_Send($j, 1, MPI_INT, id-1, 7, MPI_COMM_WORLD); //REVIEW TAG;

	}
	else{
		cout << result.size() << endl;
		cout << result << endl;
	}

}




void printResult(int id, int p){

	int i = N_LENGTH; 
	int j = M_LENGTH; //LAST POSITION
	string result;
	string final_result;
	int length;

	if(id == p){ //starts with the last processor!
		computeResult(j, id);
	}
	else if(id == 0){ // IS MAIN PROCESSOR
		for(int k = p; i > 0; i++){
			MPI_Recv($length, 1, MPI_INT, p, 5, MPI_COMM_WORLD, &status ); //RECV STRLEN FROM 3 PROCESSORS
			MPI_Recv(result, lenght, MPI_CHAR, p, 6, MPI_COMM_WORLD, &status); //RECV STR FROM 3 PROCESSORS
			final_result.insert(0, result); //JOIN ALL STR;
		}

		MPI_Recv(&j, 1, MPI_INT, id-1, 7, MPI_COMM_WORLD, $status);//RECEIVE COLUMN
		computeResult(j, id);//PERFORM ALGORITHM.

	}
	else{
		MPI_Recv(&j, 1, MPI_INT, id-1, 7, MPI_COMM_WORLD, $status);//RECEIVE COLUMN
		computeResult(j, id);//PERFORM ALGORITHM.
	}

}


