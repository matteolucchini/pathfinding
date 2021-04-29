#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
  float f;
  //Pair parent;
} Node;

// funzione che calcola i nodi vicini, per ogni nodo
Node *setNearNodes(Node grid[ROW][COL], Node q) {
  static Node nearNodes[N_DIRECTION];
  int x = q.x - 1;
  int y = q.y - 1;
  int c = 0;
  for(int i = x; i <= x + 2; i++) {
    for(int j = y; j <= y + 2; j++) {
      if((i >= 0 && i < COL) && (j >= 0 && j < ROW)) {
      	if (!(i == q.x && j == q.y)) {
        	nearNodes[c] = grid[i][j];
            //nearNodes[c].parent = {q.x, q.y};
      		c++;
		    }
      } else {
        Node tmp = {i, j, BLOCK_NODE};
        nearNodes[c] = tmp;
        c++;
      }
    }
  }
  return nearNodes;
}

bool isInList(Node node, Node *list) {
    for(int i = 0; i < sizeof(list); i++) {
        if(list[i].x == node.x && list[i].y == node.y && node.f > list[i].f)
            return true;
    }
    return false;
}

 // aggiunge i nodi in una lista
void addNode(Node *list, Node node, int *counter, int *dim) {
  *counter += 1;
  //printf("%d\n", *counter);
  if(dim < counter) {
    *dim = pow(*counter, 2);
    list = realloc(list, *dim * sizeof(Node));
  }

  list[*counter - 1] = node;
  // if(dim > counter)
  //   list = realloc(list, counter * sizeof(Node));
}

  // Return using the distance formula
float calculateHValue(Node current, Node dest){
  return (float)sqrt(pow(current.x - dest.x, 2) + pow(current.y - dest.y, 2));
}

// d)
bool computeNearNodes(Node *nearNodes, Node dst, Node q, Node *openList, Node *closedList, int *counter, int *dim){
    for(int i = 0; i < N_DIRECTION; i++) {
        if(nearNodes[i].cost != BLOCK_NODE) {
          if(nearNodes[i].x == dst.x && nearNodes[i].y == dst.y) {
            printf("ARRIVATO YUHUUU!\n");
            return true;
          }
          nearNodes[i].f = q.cost + nearNodes[i].cost + calculateHValue(nearNodes[i], dst);
          if(!isInList(nearNodes[i], closedList)) {
            addNode(openList, nearNodes[i], counter, dim);
          }
        }
    }
    return false;
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


// rimuove i nodi dalla lista
void rmNode(Node *list, Node node, int *rm_index, int *counter) {
	Node tmp;
	tmp = list[*rm_index];
	list[*rm_index] = list[*counter-1];
	list[*counter-1] = tmp;
	*counter-=1;
	list = realloc(list, *counter * sizeof(Node));
}


// float calculateGValue(Node *closedList, int countClosed) {
//   float totCost = 0;
//   for(int i = 0; i < countClosed; i++)
//     totCost += closedList[i].cost;

//   return totCost;
// }

// f = g + h
// float calculateFValue(Node current, Node dest, Node *closedList, int countClosed) {
//   return current.f = (float)(calculateGValue(closedList, countClosed) + calculateHValue(current, dest));
// }



// A* algorithm
void aStarSearch(Node grid[ROW][COL], Node src, Node dst) {
  int dimOpen = 1;
  int dimClosed = 1;
  int countOpen = 0;
  int countClosed = 0;
  Node *openList = (Node*) malloc(dimOpen * sizeof(Node));
  Node *closedList = (Node*) malloc(dimClosed * sizeof(Node));
  Node *nearNodes;
  Node q;
  float f = 0; // g + h
  //float g = 0; // movement cost to move from the starting point to a given square on the grid, following the path generated to get there
  //float h = 0; // heuristic function
  float f_min;
  int rm_index;

  // add starting node to open list
  src.f = 0;
  addNode(openList, src, &countOpen, &dimOpen);

  // se la openList non è vuota:
  while(countOpen != 0) {
    // a,b)
    f_min = src.f;
    rm_index = 0;
    for(int i = 1; i < countOpen; i++) {
      f = openList[i].f;
      if(f < f_min) {
        f_min = f;
        rm_index = i;
      }
    }
    q = openList[rm_index];
    rmNode(openList, q, &rm_index, &countOpen);
    // c)
    nearNodes = setNearNodes(grid, q);
    if(computeNearNodes(nearNodes, dst, q, openList, closedList, &countOpen, &dimOpen))
      return;
    addNode(closedList, q, &countClosed, &dimClosed);
  }
  printf("************\n");
  printf("Starting node: (%d, %d) --> cost: %f\n", src.x, src.y, src.cost);
  for(int i = 0; i < sizeof(nearNodes); i++) {
    printf("(%d, %d) --> cost: %f\n", nearNodes[i].x, nearNodes[i].y, nearNodes[i].cost);
  }
  printf("************\n");
/*
    printf("q --> x: %d, y: %d\n", q.x, q.y);
    if(q.x == dst.x && q.y == dst.y) {
      printf("Percorso migliore trovato.\n");
      printf("Costo: %f\n", f_min);
      return;
    } else {
      addNode(closedList, q, &countClosed, &dimClosed);
      rmNode(openList, q, &rm_index, &countOpen);
      // print lists
      if(countClosed != 0)
        for(int i = 0; i < countClosed; i++)
          printf("CLOSED x: %d, y: %d\n", closedList[i].x, closedList[i].y);
      else
        printf("CLOSED: empty\n");
      if(countOpen != 0)
      	for(int i = 0; i < countOpen; i++)
          	printf("OPEN x: %d, y: %d\n", openList[i].x, openList[i].y);
      else
        printf("OPEN: empty\n");
    }
*/
  free(openList);
  free(closedList);
}

int main(int argc, char *argv[]) {
  Node grid[ROW][COL];
  srand(time(0));
  initNodes(grid);
  aStarSearch(grid, grid[0][0], grid[1][1]);
  return 0;
}
