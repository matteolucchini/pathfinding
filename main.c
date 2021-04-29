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

bool isInList(Node node, Node *list, int *counter) {
    for(int i = 0; i < *counter; i++) {
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
bool computeNearNodes(Node *nearNodes, Node dst, Node q, Node *openList, Node *closedList, int *counterOpen, int *counterClosed, int *dimOpen){
    for(int i = 0; i < N_DIRECTION; i++) {
        if(nearNodes[i].cost != BLOCK_NODE) {
          if(nearNodes[i].x == dst.x && nearNodes[i].y == dst.y) {
            printf("ARRIVATO YUHUUU!\n");
            return true;
          }
          nearNodes[i].f = q.cost + nearNodes[i].cost + calculateHValue(nearNodes[i], dst);
          if(!isInList(nearNodes[i], closedList, counterClosed)) {
            addNode(openList, nearNodes[i], counterOpen, dimOpen);
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
    printf("\n");
}


// rimuove i nodi dalla lista
void rmNode(Node** list, Node node, int *rm_index, int *counter) {
	Node* tmp = malloc((*counter - 1) * sizeof(Node));
	memmove(
            tmp,
            *list,
            (*rm_index+1)*sizeof(Node));
    memmove(
            tmp+*rm_index,
            (*list)+(*rm_index+1),
            (*counter - *rm_index)*sizeof(Node));
            
    *counter -= 1;

    *list = tmp;
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
    printf("(%d,%d) %f\n", q.x, q.y, q.cost);
    rmNode(&openList, q, &rm_index, &countOpen);
    // c)
    nearNodes = setNearNodes(grid, q);
    if(computeNearNodes(nearNodes, dst, q, openList, closedList, &countOpen, &countClosed, &dimOpen))
      return;
    addNode(closedList, q, &countClosed, &dimClosed);
  }
  printf("************\n");
  printf("Starting node: (%d, %d) --> cost: %f\n", src.x, src.y, src.cost);
  for(int i = 0; i < sizeof(nearNodes); i++) {
    printf("(%d, %d) --> cost: %f\n", nearNodes[i].x, nearNodes[i].y, nearNodes[i].cost);
  }
  printf("************\n");

  free(openList);
  free(closedList);
}

int main(int argc, char *argv[]) {
  if(argc == 5) {
    Node grid[ROW][COL];
    int i = 0;
    Pair src = {atoi(argv[1]), atoi(argv[2])};
    Pair dst = {atoi(argv[3]), atoi(argv[4])};
    srand(time(0));
    initNodes(grid);
    while(grid[src.x][src.y].cost == BLOCK_NODE || grid[dst.x][dst.y].cost == BLOCK_NODE) {
      i++;
      initNodes(grid);
    }
    aStarSearch(grid, grid[src.x][src.y], grid[dst.x][dst.y]);
    printf("GRIGLIA RIFATTA: %d VOLTE\n", i);
    return 0;
  } else {
    printf("Wrong parameters\n");
    return 1;
  }
}
