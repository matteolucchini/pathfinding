#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <omp.h>
#define DEBUG true
// #define ROW 10
// #define COL 10
#define BLOCK_NODE 0
#define N_DIRECTION 8   // This project was thought with 8 directions in mind, DON'T EDIT THIS VALUE. 
                        // If you really want to edit it anyway, good luck and many sons.

// Definition of the structure of a pair
typedef struct pair {
    int x;
    int y;
}
Pair;

// Definition of the structure of a node
typedef struct node {
    int x;
    int y;
    float g;
    float h;
    float f;
    Pair parent;
}
Node;

int row, col;

// This returns all the neighboring nodes of a given node q
Pair * setNearNodes(int * grid, Pair q, int * c) {
    Pair * nearNodes = malloc(N_DIRECTION * sizeof(Pair));
    int count = 0;
    int x = q.x - 1;
    int y = q.y - 1;
    for (int i = x; i <= x + 2; i++) {
        for (int j = y; j <= y + 2; j++) {
            if ((i >= 0 && i < col) && (j >= 0 && j < row)) {
                if (!(i == q.x && j == q.y) && grid[i*row + j] == 1) {
                    Pair tmp = {
                        i,
                        j
                    };
                    nearNodes[count] = tmp;
                    count += 1;
                }
            }
        }
    }
    * c = count;
    return nearNodes;
}

// This checks if a node is inside a list
bool isInList(Pair node, Pair * list, int counter) {
    for (int i = 0; i < counter; i++) {
        if (list[i].x == node.x && list[i].y == node.y)
            return true;
    }
    return false;
}

// This adds a node in a (dynamic) array (ex. openList)
void addNode(Pair ** list, Pair node, int * counter, int * dim) {
    int d = * dim;
    int c = * counter + 1;

    if (d <= c) {
        d *= 2;
        * dim = d;
        * list = realloc( * list, d * sizeof(Pair));
        if (list == NULL) exit(15);
    }

    ( * list)[c - 1] = node;
    * counter = c;
}

// This returns the h cost of a node by using the Euclidean formula
float calculateHValue(Pair current, Pair dest) {
    return (float) sqrt(pow(current.x - dest.x, 2) + pow(current.y - dest.y, 2));
}

// This initializes each node of the grid
void initNodes(int * grid, Node * details, Pair src) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if ((rand() & 1) | (rand() & 1)) { // gives 1 with probability of 75%, gives 0 with probability of 25%
                if (i == src.x && j == src.y) {
                    details[i*row + j].g = 0;
                    details[i*row + j].h = 0;
                    details[i*row + j].f = 0;
                    details[i*row + j].parent = (Pair) {
                        i, j
                    };

                } else {
                    details[i*row + j].g = FLT_MAX;
                    details[i*row + j].h = FLT_MAX;
                    details[i*row + j].f = FLT_MAX;
                    details[i*row + j].parent = (Pair) {
                        -1, -1
                    };
                }
                grid[i*row + j] = 1;
            }

            details[i*row + j].x = i;
            details[i*row + j].y = j;
            
        }
    }
}

void readMatrix(int * grid, Node * details, Pair src) {
	printf("Reading the file...\n");
	FILE * f;
	f = fopen("matrix.txt","r");
	int c;
	int i = 0;
	int j = 0;
	c = fgetc(f);
	while(c != EOF) {
		if(c != '\n'){
			grid[i*row + j] = c - 48;
			j++;
		} else {
			i++;
			j = 0;
		}
		c = fgetc(f);
	}
	fclose(f);
	
	/*
	for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
        	printf("%d",grid[i*ROW + j]);
		}
		printf("\n");
	}
	*/
	printf("Done!\n");	
	
	
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (grid[i*row + j]) { // gives 1 with probability of 75%, gives 0 with probability of 25%
                if (i == src.x && j == src.y) {
                    details[i*row + j].g = 0;
                    details[i*row + j].h = 0;
                    details[i*row + j].f = 0;
                    details[i*row + j].parent = (Pair) {
                        i, j
                    };

                } else {
                    details[i*row + j].g = FLT_MAX;
                    details[i*row + j].h = FLT_MAX;
                    details[i*row + j].f = FLT_MAX;
                    details[i*row + j].parent = (Pair) {
                        -1, -1
                    };
                }
            }

            details[i*row + j].x = i;
            details[i*row + j].y = j;
            
        }
    }
}

// This "removes" a node in the list, by shifting back all the values after rm_index-th element
void rmNode(Pair * list, int rm_index, int * counter) {
    Pair tmp;
    int c = * counter;
    for (int i = rm_index; i < c - 1; i++) {
        tmp = list[i + 1];
        list[i] = tmp;
    }
    c--;
    * counter = c;
}

void swap(Pair * array, int l, int r) {
    Pair tmp = array[l];
    array[l] = (array)[r];
    array[r] = tmp;
}

// (Source wikiversity) This orders the array in decrescent order wrt the cost f
void quickSort(Pair * array, Node * details, int begin, int end) {
    float pivot;
    int l, r;
    if (end > begin) {
        pivot = details[array[begin].x*row + array[begin].y].f;
        l = begin + 1;
        r = end + 1;
        while (l < r)
            if (details[array[l].x*row + array[l].y].f < pivot)
                l++;
            else {
                r--;
                swap(array, l, r);
            }
        l--;
        swap(array, begin, l);
        quickSort(array, details, begin, l);
        quickSort(array, details, r, end);
    }
}

// This prints just the map
// Abbandoned, since we are dealing with really big matrices. So it is obsolete and may not work
// void printMap(int * grid, Pair * path, int cPath) {
//     Pair tmp;
//     char * map = malloc((ROW*COL+ROW+1)*sizeof(char));
//     int c = 0;
//     for (int i = 0; i < ROW; i++) {
//         for (int j = 0; j < COL; j++) {
//             if (grid[i*ROW + j] == 0) 
//                 map[c] = 0xdb;
//             else {
//                 tmp = (Pair) {
//                     i,
//                     j
//                 };
//                 if (path!=NULL && isInList(tmp, path, cPath)) map[c] = 'x';
//                 else map[c] = '.';
//             }
//             c++;
//         }
//         map[c] = '\n';
//         c++;
//     }
//     map[c] = '\0';
//     printf("%s", map);
// }

// This prints and returns the path that has been eventually found (NOT THE MAP)
Pair * printPath(Node * details, Pair dst, int * cPath) {
    int dim = 1;
    int count = * cPath;
    Pair current = dst;
    Pair * path = malloc(dim * sizeof(Pair));
    addNode( & path, current, & count, & dim);
    while (!(current.x == details[current.x*row + current.y].parent.x && current.y == details[current.x*row + current.y].parent.y)) {
        current = details[current.x*row + current.y].parent;
        addNode( & path, current, & count, & dim);
    }

    printf("Chosen path:\n");
    for (int i = count - 1; i >= 0; i--) {
        printf("\t(%d, %d)\n", path[i].x, path[i].y);
    }

    * cPath = count;
    return path;
}

// A* algorithm main function
void aStarSearch(int * grid, Node * details, Pair src, Pair dst) {
    int dimOpen = 1;
    int dimClosed = 1;
    int countOpen = 0;
    int countClosed = 0;
    int c = 0;
    float gNew;
    float hNew;
    float fNew;
    Pair * openList = malloc(dimOpen * sizeof(Pair));
    Pair * closedList = malloc(dimClosed * sizeof(Pair));
    Pair * nearNodes = NULL;
    Pair q;

    // Add starting node to open list
    addNode( & openList, src, & countOpen, & dimOpen);

    // While openList is not empty, do this
    while (countOpen != 0) {
        quickSort(openList, details, 0, countOpen - 1);
        q = openList[0];
        rmNode(openList, 0, & countOpen);
        addNode( & closedList, q, & countClosed, & dimClosed);
        nearNodes = setNearNodes(grid, q, & c);

        for (int i = 0; i < c; i++) {
            if (nearNodes[i].x == dst.x && nearNodes[i].y == dst.y) {
                addNode( & closedList, dst, & countClosed, & dimClosed);
                details[dst.x*row + dst.y].parent = q;
                printf("ARRIVED! YUHUUU!\n");
                printf("Cost: %.3f\n", details[q.x*row + q.y].g + 1);
                int cPath = 0;
                Pair * path = printPath(details, dst, & cPath);
                //printMap(grid, details, path, cPath);

                free(path);
                free(openList);
                free(closedList);
                free(nearNodes);
                putchar('\a');
                return;
            }
            if (!isInList(nearNodes[i], closedList, countClosed)) {
                gNew = details[q.x*row + q.y].g + 1.0;
                hNew = calculateHValue(nearNodes[i], dst);
                fNew = gNew + hNew;
                if (details[nearNodes[i].x*row + nearNodes[i].y].f == FLT_MAX || details[nearNodes[i].x*row + nearNodes[i].y].f > fNew) {
                    addNode( & openList, nearNodes[i], & countOpen, & dimOpen);

                    details[nearNodes[i].x*row + nearNodes[i].y].f = fNew;
                    details[nearNodes[i].x*row + nearNodes[i].y].g = gNew;
                    details[nearNodes[i].x*row + nearNodes[i].y].h = hNew;
                    details[nearNodes[i].x*row + nearNodes[i].y].parent = q;
                }
            }
        }
        free(nearNodes);
        nearNodes = NULL;
    }
    printf("Impossible to reach the destination!\n");
    //printMap(grid, details, NULL, 0);
    free(openList);
    free(closedList);
    free(nearNodes);
    putchar('\a');
}

int main(int argc, char * argv[]) {
    if (argc >= 7) {
        row = atoi(argv[5]);
        col = atoi(argv[6]);
        Pair src = {
            atoi(argv[1]),
            atoi(argv[2])
        };
        Pair dst = {
            atoi(argv[3]),
            atoi(argv[4])
        };
        if (src.x == dst.x && src.y == dst.y) {
            printf("The starting node is the same of the destination one.\n");
            return 2;
        }
        if ((src.x < 0 || src.x >= col) || (src.y < 0 || src.y >= row) || (dst.x < 0 || dst.x >= col) || (dst.y < 0 || dst.y >= row)) {
            printf("Wrong starting or destination node.\n");
            return 3;
        }
        int * grid = calloc(row*col, sizeof(int));
        Node *  details = malloc(row*col * sizeof(Node));
        srand(time(0));
        if(DEBUG)
        	readMatrix(grid, details, src);
        else{
	        initNodes(grid, details, src);
	        while (grid[(src.x*row) + src.y] == BLOCK_NODE || grid[(dst.x*row) + dst.y] == BLOCK_NODE) {
	            initNodes(grid, details, src);
	        }
		}
        double begin = omp_get_wtime();
        aStarSearch(grid, details, src, dst);
        double end = omp_get_wtime();
        printf("Time: %f (s)\n", end-begin);
        free(grid);
        free(details);
        return 0;
    } else {
        printf("Wrong parameters.\n");
        return 1;
    }
}