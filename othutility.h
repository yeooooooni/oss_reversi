#include <stdio.h>
#include <stdlib.h>

typedef enum stone_t {BLACK = -1, WHITE = 1} stone;

typedef struct {
        char name[BUFSIZ];
        int stone; // or char* stone;
} player;

void initializeArraytoAllZeros(int array[8][8]);
int findPossible(stone currentStoneColor, int othelloBoard[8][8], int possiblePositionBoard[8][8]);
void putStone(stone currentStoneColor, int othelloBoard[8][8], int userCoordinateX, int userCoordinateY);

