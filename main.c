#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <omp.h>
#define TASK_SIZE 100
#define ROW 500
#define COL 500
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

// This returns all the neighboring nodes of a given node q
Pair * setNearNodes(int grid[ROW][COL], Node details[ROW][COL], Pair q, int * c) {
    Pair * nearNodes = malloc(N_DIRECTION * sizeof(Pair));
    int count = 0;
    int x = q.x - 1;
    int y = q.y - 1;
    for (int i = x; i <= x + 2; i++) {
        for (int j = y; j <= y + 2; j++) {
            if ((i >= 0 && i < COL) && (j >= 0 && j < ROW)) {
                if (!(i == q.x && j == q.y) && grid[i][j] == 1) {
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
// bool isInList(Pair node, Pair * list, int counter) {
//     bool t = false;
//     omp_set_num_threads(8);
//     #pragma omp parallel
//     {
//         for (int i = 0; i < counter; i++) {
//             if (list[i].x == node.x && list[i].y == node.y)
//                 t = true;
//         }
//     }
//     return t;
// }
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
void initNodes(int grid[ROW][COL], Node details[ROW][COL], Pair src) {
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if ((rand() & 1) | (rand() & 1)) { // gives 1 with probability of 75%, gives 0 with probability of 25%
                if (i == src.x && j == src.y) {
                    details[i][j].g = 0;
                    details[i][j].h = 0;
                    details[i][j].f = 0;
                    details[i][j].parent = (Pair) {
                        i, j
                    };

                } else {
                    details[i][j].g = FLT_MAX;
                    details[i][j].h = FLT_MAX;
                    details[i][j].f = FLT_MAX;
                    details[i][j].parent = (Pair) {
                        -1, -1
                    };
                }
                grid[i][j] = 1;
            } else {
                grid[i][j] = BLOCK_NODE;
            }

            details[i][j].x = i;
            details[i][j].y = j;
            
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
void quickSort(Pair * array, Node details[ROW][COL], int begin, int end) {
    float pivot;
    int l, r;
    if (end > begin) {
        pivot = details[array[begin].x][array[begin].y].f;
        l = begin + 1;
        r = end + 1;
        while (l < r)
            if (details[array[l].x][array[l].y].f < pivot)
                l++;
            else {
                r--;
                swap(array, l, r);
            }
        l--;
        swap(array, begin, l);
        #pragma omp task shared(array) if(end - begin > TASK_SIZE)
        quickSort(array, details, begin, l);
        #pragma omp task shared(array) if(end - begin > TASK_SIZE)
        quickSort(array, details, r, end);
    }
}

// This prints just the map
void printMap(int grid[ROW][COL], Node details[ROW][COL], Pair * path, int cPath) {
    Pair tmp;
    char map[ROW*COL+ROW];
    int c = 0;
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if (grid[i][j] == 0) 
                map[c] = 0xdb;
            else {
                tmp = (Pair) {
                    i,
                    j
                };
                if (path!=NULL && isInList(tmp, path, cPath)) map[c] = 'x';
                else map[c] = '.';
            }
            c++;
        }
        map[c] = '\n';
        c++;
    }
	
	map[c] = '\0';
    printf("%s", map);
}

// This prints and returns the path that has been eventually found (NOT THE MAP)
Pair * printPath(int grid[ROW][COL], Node details[ROW][COL], Pair dst, int * cPath) {
    int dim = 1;
    int count = * cPath;
    Pair current = dst;
    Pair * path = malloc(dim * sizeof(Pair));
    addNode( & path, current, & count, & dim);
    while (!(current.x == details[current.x][current.y].parent.x && current.y == details[current.x][current.y].parent.y)) {
        current = details[current.x][current.y].parent;
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
void aStarSearch(int grid[ROW][COL], Node details[ROW][COL], Pair src, Pair dst) {
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
    
    omp_set_dynamic(0);              // Explicitly disable dynamic teams
    omp_set_num_threads(4); // Use N threads for all parallel regions

    // Add starting node to open list
    addNode( & openList, src, & countOpen, & dimOpen);

    // While openList is not empty, do this
    while (countOpen != 0) {
    	#pragma omp parallel
    	{
    		#pragma omp single
    		quickSort(openList, details, 0, countOpen - 1);
        }
        q = openList[0];
        rmNode(openList, 0, & countOpen);
        addNode( & closedList, q, & countClosed, & dimClosed);
        nearNodes = setNearNodes(grid, details, q, & c);

        for (int i = 0; i < c; i++) {
            if (nearNodes[i].x == dst.x && nearNodes[i].y == dst.y) {
                addNode( & closedList, dst, & countClosed, & dimClosed);
                details[dst.x][dst.y].parent = q;
                printf("ARRIVED! YUHUUU!\n");
                printf("Cost: %.3f\n", details[q.x][q.y].g + 1);
                int cPath = 0;
                Pair * path = printPath(grid, details, dst, & cPath);
                printMap(grid, details, path, cPath);

                free(path);
                free(openList);
                free(closedList);
                free(nearNodes);
                putchar('\a'); // funny thing
                return;
            }
            if (!isInList(nearNodes[i], closedList, countClosed)) {
                gNew = details[q.x][q.y].g + 1.0;
                hNew = calculateHValue(nearNodes[i], dst);
                fNew = gNew + hNew;
                if (details[nearNodes[i].x][nearNodes[i].y].f == FLT_MAX || details[nearNodes[i].x][nearNodes[i].y].f > fNew) {
                    addNode( & openList, nearNodes[i], & countOpen, & dimOpen);

                    details[nearNodes[i].x][nearNodes[i].y].f = fNew;
                    details[nearNodes[i].x][nearNodes[i].y].g = gNew;
                    details[nearNodes[i].x][nearNodes[i].y].h = hNew;
                    details[nearNodes[i].x][nearNodes[i].y].parent = q;
                }
            }
        }
        free(nearNodes);
        nearNodes = NULL;
    }
    printf("Impossible to reach the destination!\n");
    printMap(grid, details, NULL, 0);
    free(openList);
    free(closedList);
    free(nearNodes);
}

int main(int argc, char * argv[]) {
    if (argc >= 5) {
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
        if ((src.x < 0 || src.x >= COL) || (src.y < 0 || src.y >= ROW) || (dst.x < 0 || dst.x >= COL) || (dst.y < 0 || dst.y >= ROW)) {
            printf("Wrong starting or destination node.\n");
            return 3;
        }
        int grid[ROW][COL];
        Node details[ROW][COL];
        srand(time(0));
        initNodes(grid, details, src);
        while (grid[src.x][src.y] == BLOCK_NODE || grid[dst.x][dst.y] == BLOCK_NODE) {
            initNodes(grid, details, src);
        }
		double begin = omp_get_wtime();
        aStarSearch(grid, details, src, dst);
        double end = omp_get_wtime();
        printf("Time: %f (s) \n", end-begin);
        return 0;
    } else {
        printf("Wrong parameters.\n");
        return 1;
    }
}