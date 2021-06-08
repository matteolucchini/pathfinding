#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <omp.h>
#define READ false 		// Set this to true if you want to read the matrix from a file "matrix.txt".
#define BLOCK_NODE 0
#define N_DIRECTION 8   // This project is thought with 8 directions in mind, DON'T EDIT THIS VALUE. 
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
                if (!(i == q.x && j == q.y) && grid[i * row + j] == 1) {
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

// This initializes the grid
void initGrid(int * grid) {
    #pragma omp parallel for
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            // Gives 1 with probability of 75%
            if ((rand() & 1) | (rand() & 1)) {
                grid[i * row + j] = 1;
            }
        }
    }
}

// This initializes each node of the grid
void initNodes(int * grid, Node * details, Pair src_new, Pair src) {
    #pragma omp parallel for
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < row; j++) {
            int tmp = i * row + j;
            if (grid[i * row + j]) {
                if (i == src_new.x && j == src_new.y) {
                    details[tmp].g = 1;
                    details[tmp].h = 0;
                    details[tmp].f = 0;
                    details[tmp].parent = src;

                } else {
                    details[tmp].g = FLT_MAX;
                    details[tmp].h = FLT_MAX;
                    details[tmp].f = FLT_MAX;
                    details[tmp].parent = (Pair) {
                        -1, -1
                    };
                }
            }
            details[tmp].x = i;
            details[tmp].y = j;
        }
    }
}

// Read matrix from file
void readMatrix(int * grid) {
    printf("Reading the file for the map... ");
    FILE * f;
    f = fopen("matrix.txt", "r");
    int c;
    int i = 0;
    int j = 0;
    c = fgetc(f);
    while (c != EOF) {
        if (c != '\n') {
            grid[i * row + j] = c - 48;
            j++;
        } else {
            i++;
            j = 0;
        }
        c = fgetc(f);
    }
    fclose(f);
    printf("Done!\n");
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
        pivot = details[array[begin].x * row + array[begin].y].f;
        l = begin + 1;
        r = end + 1;
        while (l < r)
            if (details[array[l].x * row + array[l].y].f < pivot)
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
// Abandoned, since we are dealing with really big matrices. 
// So it is obsolete and may not work if used.
void printMap(int * grid, Pair * path, int cPath) {
    Pair tmp;
    char * map = malloc((row * col + row + 1) * sizeof(char));
    int c = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (grid[i * row + j] == 0)
                map[c] = 0xdb;
            else {
                tmp = (Pair) {
                    i,
                    j
                };
                if (path != NULL && isInList(tmp, path, cPath)) map[c] = 'x';
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
// Abandoned, since we are dealing with really big matrices. 
// So it is obsolete and may not work if used.
Pair * printPath(Node * details, Pair dst, int * cPath) {
    int dim = 1;
    int count = * cPath;
    Pair current = dst;
    Pair * path = malloc(dim * sizeof(Pair));
    addNode( & path, current, & count, & dim);
    int tmp = current.x * row + current.y;
    while (!(current.x == details[tmp].parent.x && current.y == details[tmp].parent.y)) {
        current = details[tmp].parent;
        addNode( & path, current, & count, & dim);
    }

    * cPath = count;
    return path;
}

// A* algorithm main function
void aStarSearch(int * grid, Pair src, Pair dst) {
    Pair * nearNodes = NULL;
    int c = 0;
    nearNodes = setNearNodes(grid, src, & c);
    Node * details[c];
    Pair src_new[c];
    printf("Initializing the nodes for each thread... ");
    #pragma omp parallel for
    for (int i = 0; i < c; i++) {
        details[i] = malloc(row * col * sizeof(Node));
        src_new[i] = nearNodes[i];
        initNodes(grid, details[i], src_new[i], src);
    }
    printf("Done!\n");
    free(nearNodes);

    printf("Start!\n");
    double begin = omp_get_wtime();

    // Opens a parallel region
    #pragma omp parallel 
    {
        // Let one thread create all the eventual threads 
        #pragma omp single nowait 
        {
            for (int j = 0; j < c; j++) {
                // Create thread for each node in nearNodes
                #pragma omp task 
                {
                    int dimOpen = 1;
                    int dimClosed = 1;
                    int countOpen = 0;
                    int countClosed = 0;
                    int c = 0;
                    float gNew;
                    float hNew;
                    float fNew;
                    double begin;
                    double end;
                    bool found = false;
                    Node * details_ptr = details[j];
                    Pair * nearNodes = NULL;
                    Pair * openList = malloc(dimOpen * sizeof(Pair));
                    Pair * closedList = malloc(dimClosed * sizeof(Pair));
                    Pair q;
                    addNode( & closedList, src, & countClosed, & dimClosed);
                    addNode( & openList, src_new[j], & countOpen, & dimOpen);
                    begin = omp_get_wtime();
                    // While openList is not empty, do this
                    while (countOpen != 0) {
                        // Can't make respect this bool in the while. Am I missing something?
                        if (found) break;
                        quickSort(openList, details_ptr, 0, countOpen - 1);
                        q = openList[0];
                        rmNode(openList, 0, & countOpen);
                        addNode( & closedList, q, & countClosed, & dimClosed);
                        nearNodes = setNearNodes(grid, q, & c);

                        for (int i = 0; i < c; i++) {
                            if (nearNodes[i].x == dst.x && nearNodes[i].y == dst.y) {
                                addNode( & closedList, dst, & countClosed, & dimClosed);
                                details_ptr[dst.x * row + dst.y].parent = q;
                                end = omp_get_wtime();
                                printf("Thread %d: path found! \t Time: %f (s) \t Cost: %.3f\n", omp_get_thread_num(), end - begin, details_ptr[q.x * row + q.y].g + 1);
                                found = true;
                            }
                            if (!isInList(nearNodes[i], closedList, countClosed)) {
                                gNew = details_ptr[q.x * row + q.y].g + 1.0;
                                hNew = calculateHValue(nearNodes[i], dst);
                                fNew = gNew + hNew;
                                int tmp = nearNodes[i].x * row + nearNodes[i].y;
                                if (details_ptr[tmp].f == FLT_MAX || details_ptr[tmp].f > fNew) {
                                    addNode( & openList, nearNodes[i], & countOpen, & dimOpen);
                                    int tmp2 = nearNodes[i].x * row + nearNodes[i].y;
                                    details_ptr[tmp2].f = fNew;
                                    details_ptr[tmp2].g = gNew;
                                    details_ptr[tmp2].h = hNew;
                                    details_ptr[tmp2].parent = q;
                                }
                            }
                        }
                        free(nearNodes);
                        nearNodes = NULL;
                    }
                    if (countOpen == 0){
			end = omp_get_wtime();
                        printf("Thread %d: path not found... \t Time: %f (s)\n", omp_get_thread_num(), end - begin);
		    }
                    free(openList);
                    free(closedList);
                    free(nearNodes);
                    free(details_ptr);
                }
            }
        }
    } // Implicit barrier
    double end = omp_get_wtime();
    printf("Finish!\nTotal time: %f (s)\n\a", end - begin);
}

int main(int argc, char * argv[]) {
    omp_set_dynamic(0); // Explicitly disable dynamic teams
    if (argc >= 8) {
        if (atoi(argv[7]) <= 0) {
            printf("Not a valid number for threads.\n");
            return 5;
        }
        omp_set_num_threads(atoi(argv[7])); // Use N threads for all parallel regions
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
        int * grid = calloc(row * col, sizeof(int));
        srand(time(0));
        if (READ) {
            readMatrix(grid);
            if (grid[(src.x * row) + src.y] == BLOCK_NODE || grid[(dst.x * row) + dst.y] == BLOCK_NODE) {
                printf("Map not feasible!\n");
                return 4;
            }
        } else {
            printf("Generating map... ");
            while (grid[(src.x * row) + src.y] == BLOCK_NODE || grid[(dst.x * row) + dst.y] == BLOCK_NODE){
            	free(grid);
            	grid = calloc(row * col, sizeof(int));
                initGrid(grid);
            }
            printf("Done!\n");
        }
        aStarSearch(grid, src, dst);
        free(grid);
        return 0;
    } else {
        printf("Wrong parameters.\n");
        return 1;
    }
}
