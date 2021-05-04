#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <float.h>

#define ROW 5 // rows of the grid
#define COL 5 // cols of the grid
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
Pair * setNearNodes(int grid[ROW][COL], Node details[ROW][COL], Pair * nearNodes, Pair q, int * c) {
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
                    nearNodes[ * c] = tmp;
                    details[i][j].parent = q;
                    * c += 1;
                }
            }
        }
    }
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
void addNode(Pair * list, Pair node, int * counter, int * dim) {
    * counter += 1;
    printf("\ncounter: %d, dim: %d\n", *counter, *dim);
    if (*dim < *counter) {
        * dim = pow( * counter, 2);
        list = realloc(list, * dim * sizeof(Pair));
    }

    list[ * counter - 1] = node;
    // if(dim > counter)
    //   list = realloc(list, counter * sizeof(Node));
}

// Return using the distance formula
float calculateHValue(Pair current, Pair dest) {
    return (float) sqrt(pow(current.x - dest.x, 2) + pow(current.y - dest.y, 2));
}

// d)
bool computeNearNodes(Node details[ROW][COL], Pair * nearNodes, int c, Pair dst, Pair q, Pair * openList, Pair * closedList, int * counterOpen, int * counterClosed, int * dimOpen) {
    float gNew;
    float hNew;
    float fNew;

    for (int i = 0; i < c; i++) {
        if (nearNodes[i].x == dst.x && nearNodes[i].y == dst.y) {
            printf("ARRIVATO YUHUUU!\n");
            return true;
        }
        if (!isInList(nearNodes[i], closedList, counterClosed)) {
            gNew = details[q.x][q.y].g + 1.0;
            hNew = calculateHValue(nearNodes[i], dst);
            fNew = gNew + hNew;
            if (details[nearNodes[i].x][nearNodes[i].y].f == FLT_MAX || details[nearNodes[i].x][nearNodes[i].y].f > fNew) {
                addNode(openList, nearNodes[i], counterOpen, dimOpen);
                details[nearNodes[i].x][nearNodes[i].y].f = fNew;
                details[nearNodes[i].x][nearNodes[i].y].g = gNew;
                details[nearNodes[i].x][nearNodes[i].y].h = hNew;
                details[nearNodes[i].x][nearNodes[i].y].parent = q;
            }
        }
    }
    return false;
}

void initNodes(int grid[ROW][COL], Node details[ROW][COL], Pair src) {
    Pair tmp = {
        -1,
        -1
    };
    for (int i = 0; i < ROW; i++) {
        for (int j = 0; j < COL; j++) {
            if ((rand() & 1) | (rand() & 1)) { // gives 1 with probability of 75%, gives 0 with probability of 25%
                if (i == src.x && j == src.y) {
                    details[i][j].g = 0;
                    details[i][j].h = 0;
                    details[i][j].f = 0;
                } else {
                    details[i][j].g = FLT_MAX;
                    details[i][j].h = FLT_MAX;
                    details[i][j].f = FLT_MAX;
                }
                details[i][j].parent = tmp;
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
void rmNode(Pair * list, int * rm_index, int * counter) {
    for (int i = * rm_index; i < * counter - 1; i++)
        list[i] = list[i + 1];
    * counter -= 1;
}

// A* algorithm
void aStarSearch(int grid[ROW][COL], Node details[ROW][COL], Pair src, Pair dst) {
    int dimOpen = 1;
    int dimClosed = 1;
    int countOpen = 0;
    int countClosed = 0;
    Pair * openList = (Pair * ) malloc(dimOpen * sizeof(Pair));
    Pair * closedList = (Pair * ) malloc(dimClosed * sizeof(Pair));
    Pair * nearNodes = (Pair * ) malloc(N_DIRECTION * sizeof(Pair));
    Pair q;
    float f_min;
    float f;
    int rm_index;

    // add starting node to open list
    addNode(openList, src, & countOpen, & dimOpen);

    // se la openList non è vuota:
    while (countOpen != 0) {
        // a,b)
        f_min = details[openList[0].x][openList[0].y].f;
        rm_index = 0;
        for (int i = 1; i < countOpen; i++) {
            f = details[openList[i].x][openList[i].y].f;
            if (f < f_min) {
                f_min = f;
                rm_index = i;
            }
        }
        q = openList[rm_index];
        addNode(closedList, q, & countClosed, & dimClosed);
        printf("(%d,%d)\n", q.x, q.y);
        // c)
        int c = 0;
        nearNodes = setNearNodes(grid, details, nearNodes, q, & c);
        if (computeNearNodes(details, nearNodes, c, dst, q, openList, closedList, & countOpen, & countClosed, & dimOpen)) {
            free(openList);
            free(closedList);
            free(nearNodes);
            return;
        }
        rmNode(openList, & rm_index, & countOpen);
        printf("%d", countOpen);
    }

    printf("\nNope\n");
    free(openList);
    free(closedList);
    free(nearNodes);
}

int main(int argc, char * argv[]) {
    if (argc == 5) {
        int grid[ROW][COL];
        Node details[ROW][COL];
        int i = 0;
        Pair src = {
            atoi(argv[1]),
            atoi(argv[2])
        };
        Pair dst = {
            atoi(argv[3]),
            atoi(argv[4])
        };
        srand(time(0));
        initNodes(grid, details, src);
        while (grid[src.x][src.y] == BLOCK_NODE || grid[dst.x][dst.y] == BLOCK_NODE) {
            i++;
            initNodes(grid, details, src);
        }
        aStarSearch(grid, details, src, dst);
        printf("GRIGLIA RIFATTA: %d VOLTE\n", i);
        return 0;
    } else {
        printf("Wrong parameters\n");
        return 1;
    }
}