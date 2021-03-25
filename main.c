#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROW 5 // rows of the grid
#define COL 5 // cols of the grid
#define MAX_COST 10
#define MIN_COST 1
#define BLOCK_NODE -1

// Dichiarazione struttura nodo
typedef struct node {
  float cost;
  struct Node *near_nodes[];
} Node;

void randomCost(Node grid[ROW][COL]){
  float treshold; // soglia per decidere se mettere 0 o numero random
  for(int i = 0; i < ROW; i++)
    for(int j = 0; j < COL; j++) {
      if((rand() & 1) | (rand() & 1)) { // gives 1 with probability of 75%, gives 0 with probability of 25%
        // if 1
        grid[i][j].cost = MIN_COST + (float)(rand()) / (float)(RAND_MAX / (MAX_COST - MIN_COST)); // random float number from 0 to MAX_COST
      } else {
        // if 0
        grid[i][j].cost = BLOCK_NODE;
      }
    }

    for(int i = 0; i < ROW; i++) {
      for(int j = 0; j < COL; j++)
          printf("%.2f ", grid[i][j].cost);
      printf("\n");
    }
}

int main(int argc, char *argv[]) {
  Node grid[ROW][COL];
  srand(time(0));
  randomCost(grid);
  return 0;
}
