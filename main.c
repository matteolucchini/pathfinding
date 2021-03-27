#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define ROW 5 // rows of the grid
#define COL 5 // cols of the grid
#define MAX_COST 10
#define MIN_COST 1
#define BLOCK_NODE -1
#define N_DIRECTION 8

// Dichiaro coppie per rappresentare il punto con le sue coordinate
typedef struct pair {
  int x;
  int y;
} Pair;

// Dichiarazione struttura nodo
typedef struct node {
  int x;
  int y;
  float cost;
} Node;

// funzione che calcola i nodi vicini, per ogni nodo
void setNearNodes(Node grid[ROW][COL], Node src) {
  for(int i = 0; i < ROW; i++) {
    for(int j = 0; j < COL; j++) {

    }
  }
}

void initNodes(Node grid[ROW][COL]){
  for(int i = 0; i < ROW; i++) {
    for(int j = 0; j < COL; j++) {
        if((rand() & 1) | (rand() & 1)) { // gives 1 with probability of 75%, gives 0 with probability of 25%
          // if 1
          grid[i][j].cost = MIN_COST + (float)(rand()) / (float)(RAND_MAX / (MAX_COST - MIN_COST)); // random float number from 0 to MAX_COST
        }
        else {
          // if 0
          grid[i][j].cost = BLOCK_NODE;
        }
        grid[i][j].x = i;
        grid[i][j].y = j;
      }
    }
    for(int i = 0; i < ROW; i++) {
      for(int j = 0; j < COL; j++)
          printf("%.2f \t", grid[i][j].cost);
      printf("\n");
    }
}

/*
int* getptr(int argc, char *argv[]){
	int i, dimptr = 1;
	int* ptr = (int*) malloc(dimptr * sizeof(int));

	for (i = 1; i < argc; i++){
		if (dimptr < i){
			dimptr = pow(i,2);
			ptr = realloc(ptr, dimptr * sizeof(int));
		}
		ptr[i-1] = atoi(argv[i]);
	}

	if (dimptr > i)
		ptr = realloc(ptr, i * sizeof(int));

	return ptr;
}
*/

 // aggiunge i nodi in una lista
void addNode(Node *list, Node node, int *counter, int *dim) {
  *counter += 1;
  printf("%d\n", *counter);
  if(dim < counter) {
    *dim = pow(*counter, 2);
    list = realloc(list, *dim * sizeof(Node));
  }

  list[*counter - 1] = node;
  // if(dim > counter)
  //   list = realloc(list, counter * sizeof(Node));
}

void rmNode(Node *list, Node node, int *rm_index, int *counter) {
  memmove(list, list + *rm_index, (*counter - *rm_index) * sizeof(Node));
  *counter-=1
  // for(int i = 0; i < counter; i++) {
  //   if(i < rm_index)
  //     tmp_list[i] = list[i];
  //   if(i > rm_index)
  //     tmp_list[i - 1] = list[i];
  // }
}

float calculateHValue(Node current, Node dest)
{
  // Return using the distance formula
  return (float)sqrt(pow(current.x - dest.x, 2) + pow(current.y - dest.y, 2));
}

float calculateGValue(Node *closedList, int countClosed) {
  float totCost = 0;
  for(int i = 0; i < countClosed; i++)
    totCost += closedList[i].cost;

  return totCost;
}

float calculateFValue(Node current, Node dest, Node *closedList, int countClosed) {
  return (float)(calculateGValue(closedList, countClosed) + calculateHValue(current, dest));
}

// A* algorithm
void aStarSearch(Node grid[ROW][COL], Node src, Node dst) {
  int dimOpen = 1;
  int dimClosed = 1;
  int countOpen = 0;
  int countClosed = 0;
  Node *openList = (Node*) malloc(dimOpen * sizeof(Node));
  Node *closedList = (Node*) malloc(dimClosed * sizeof(Node));
  Node q;
  float f = 0; // g + h
  float g = 0; // movement cost to move from the starting point to a given square on the grid, following the path generated to get there
  float h = 0; // heuristic function
  float f_min;
  int rm_index;

  // add starting node to open list
  addNode(openList, src, &countOpen, &dimOpen);
  printf("%d", countOpen);

  while(countOpen != 0) {
    if(countOpen == 1) {
      f_min = calculateFValue(openList[0], dst, closedList, countClosed);
      q = src;
      rm_index = countOpen - 1;
    } else {
      for(int i = 1; i < countOpen; i++) {
        f = calculateFValue(openList[i], dst, closedList, countClosed);
        if(f < f_min) {
          f_min = f;
          q = openList[i];
          rm_index = i;
        }
      }
    }
    printf("q --> x: %d, y: %d", q.x, q.y);
    if(q.x == dst.x && q.y == dst.y) {
      printf("Percorso migliore trovato.\n");
      printf("Costo: %f\n", f_min);
      return;
    } else {
      addNode(closedList, q, &countClosed, &dimClosed);
      rmNode(openList, q, &rm_index, &countOpen);
      // print lists
      for(int i = 0; i < countClosed; i++) {
        printf("OPEN x: %d, y: %d\n", openList[i].x, openList[i].y);
        printf("CLOSED x: %d, y: %d\n", closedList[i].x, closedList[i].y);
      }
    }

  }



  free(openList);
  free(closedList);
}

int main(int argc, char *argv[]) {
  Node grid[ROW][COL];
  srand(time(0));
  initNodes(grid);
  aStarSearch(grid, grid[0][0], grid[4][4]);
  return 0;
}
