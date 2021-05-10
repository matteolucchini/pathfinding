#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <float.h>
#define ROW 5
#define COL 5
#define MAX_COST 10
#define MIN_COST 1
#define BLOCK_NODE 0
#define N_DIRECTION 8

// Dichiaro coppie per rappresentare il punto con le sue coordinate
typedef struct pair {
    int x;
    int y;
}
Pair;

// Dichiarazione struttura nodo
typedef struct node {
    int x;
    int y;
    float g;
    float h;
    float f;
    Pair parent;
}
Node;

// funzione che calcola i nodi vicini, per ogni nodo
Pair* setNearNodes(int grid[ROW][COL], Node details[ROW][COL], Pair q, int * c) {
    Pair* nearNodes = malloc(N_DIRECTION * sizeof(Pair));
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
                    nearNodes[count]= tmp;
                    details[i][j].parent = q;
                    count += 1;
                }
            }
        }
    }
    *c = count;
    return nearNodes;
}

bool isInList(Pair node, Pair * list, int * counter) {
    for (int i = 0; i < * counter; i++) {
        if (list[i].x == node.x && list[i].y == node.y)
            return true;
    }
    return false;
}

// aggiunge i nodi in una lista
void addNode(Pair ** list, Pair node, int * counter, int * dim) {
    int d = *dim;
    int c = *counter+1;

    if (d <= c) {
        Pair * tmp;
        d *= 2;
        *dim = d;
        *list = realloc(*list, d * sizeof(Pair));
        if(list == NULL) exit(15);
    }

    (*list)[c-1] = node;
    *counter = c;
}

// Return using the distance formula
float calculateHValue(Pair current, Pair dest) {
    return (float) sqrt(pow(current.x - dest.x, 2) + pow(current.y - dest.y, 2));
}

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
        for (int j = 0; j < COL; j++)
            printf("%d \t", grid[i][j]);
        printf("\n");
    }
    printf("\n");
}

// rimuove i nodi dalla lista
void rmNode(Pair * list, int rm_index, int * counter) {
    Pair tmp;
    int c = *counter;
    for (int i = rm_index; i < c - 1; i++) {
        tmp = list[i + 1];
        list[i] = tmp;
    }
    c--;
    *counter = c;
}

void swap(Pair * array, int l, int r) {
    Pair tmp = array[l];
    array[l] = (array)[r];
    array[r] = tmp;
}

//source wikiversity
void quickSort(Pair * array, Node details[ROW][COL], int begin, int end) {
    float pivot;
    int l, r;
    if (end > begin) {
        pivot = details[array[begin].x][array[begin].y].f;
        l = begin + 1;
        r = end + 1;
        while(l < r)
            if (details[array[l].x][array[l].y].f < pivot)
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

// A* algorithm
void aStarSearch(int grid[ROW][COL], Node details[ROW][COL], Pair src, Pair dst) {
    int dimOpen = 1;
    int dimClosed = 1;
    int countOpen = 0;
    int countClosed = 0;
    int c = 0;
    int rm_index;
    float f_min;
    float f;
    float gNew;
    float hNew;
    float fNew;
    Pair * openList = malloc(dimOpen * sizeof(Pair));
    Pair * closedList = malloc(dimClosed * sizeof(Pair));
    Pair * nearNodes = NULL;
    Pair q;

    // add starting node to open list
    addNode(&openList, src, & countOpen, & dimOpen);

    // se la openList non è vuota:
    while (countOpen != 0) {
        // a,b)
        quickSort(openList, details, 0, countOpen-1);
        q = openList[0];
        rmNode(openList, 0, & countOpen);
        addNode(&closedList, q, &countClosed, &dimClosed);
        printf("(%d,%d)\n", q.x, q.y);
        // c)
        nearNodes = setNearNodes(grid, details, q, & c);

        for (int i = 0; i < c; i++) {
            if (nearNodes[i].x == dst.x && nearNodes[i].y == dst.y) {
                printf("(%d,%d)\n", dst.x, dst.y);
                printf("ARRIVED! YUHUUU!\n");
                free(openList);
                free(closedList);
                free(nearNodes);
                return;
            }
            if (!isInList(nearNodes[i], closedList, &countClosed)) {
                gNew = details[q.x][q.y].g + 1.0;
                hNew = calculateHValue(nearNodes[i], dst);
                fNew = gNew + hNew;
                if (details[nearNodes[i].x][nearNodes[i].y].f == FLT_MAX || details[nearNodes[i].x][nearNodes[i].y].f > fNew) {
                    addNode(&openList, nearNodes[i], &countOpen, &dimOpen);

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

    printf("\nNope");
    free(openList);
    free(closedList);
    free(nearNodes);
}

int main(int argc, char * argv[]) {
    if (argc <= 5) {
        Pair src = {
            atoi(argv[1]),
            atoi(argv[2])
        };
        Pair dst = {
            atoi(argv[3]),
            atoi(argv[4])
        };
        if (src.x == dst.x && src.y == dst.y){
        	printf("The starting node is the same of the destination one.\n");
        	return 2;
		}
        int grid[ROW][COL];
        int i = 0;
        Node details[ROW][COL];
        srand(time(0));
        while (grid[src.x][src.y] == BLOCK_NODE || grid[dst.x][dst.y] == BLOCK_NODE) {
            i++;
            initNodes(grid, details, src);
        }
        aStarSearch(grid, details, src, dst);
        printf("\nGrid redone %d time(s)\n", i);
        return 0;
    } else {
        printf("Wrong parameters.\n");
        return 1;
    }
}