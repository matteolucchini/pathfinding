#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void buildMatrix(int row, int col) {
	FILE *file;
	file = fopen("matrix.txt", "w");
	if(file == NULL) {
		printf("Errore apertura file\n");
		exit(1);
	} else {
		srand(time(0));
		for (int i = 0; i < row; i++) {
	        for (int j = 0; j < col; j++) {
	            if (rand() & 1) {
	            	fprintf(file, "%d", 1);
	            } else {
	            	fprintf(file, "%d", 0);
	            }
	        }
	        fprintf(file, "\n");
	    }
	}
}

int main(int argc, char * argv[]){
	buildMatrix(atoi(argv[1]), atoi(argv[2]));
	return 0;
}
