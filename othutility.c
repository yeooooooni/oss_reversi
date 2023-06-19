#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "othutility.h"

int dx[8] = {-1, 0 ,1, 1, 1, 0, -1, -1};
int dy[8] = {-1, -1, -1, 0, 1, 1, 1, 0};


void initializeArraytoAllZeros(int array[8][8]){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            *(array[i] + j) = 0;
        }
    }
}

//return number of found possible position -> if 0 -> then return 0. 
int findPossible(stone currentStoneColor, int othelloBoard[8][8], int possiblePositionBoard[8][8]){
    int foundX;
    int foundY;
    int curX;
    int curY;
    int curVal;
    int possibleCount = 0;
    
    //find possible (store possible coordinates to possible array)
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(othelloBoard[i][j] == currentStoneColor){
                foundX = i;
                foundY = j;
                
                //explore all 8 directions from the found Stone
                for(int k = 0; k < 8; k++){
                    curX = foundX + dx[k];
                    curY = foundY + dy[k];
                    
                    if((curX < 0 || curX >= 8) || (curY < 0 || curY >= 8)){
                        continue;
                    }
                    curVal = othelloBoard[curX][curY];
                    if(curVal == 0 || curVal == currentStoneColor) {
                        continue;
                    }
                    //if it's a different colored stone, go to that direction
                    while(1){
                        curX = curX + dx[k];
                        curY = curY + dy[k];
            
                        if((curX < 0 || curX >= 8) || (curY < 0 || curY >= 8)){
                            break;
                        }
                        curVal = othelloBoard[curX][curY];
                        if(curVal == currentStoneColor){
                            break;
                        }
                        else if(curVal == 0){//means empty
                           //possiblePosition[curX][curY] = 7; // 7 means possible (made it 7 b.c it might get confusing)
                           *(possiblePositionBoard[curX] + curY) = 7;
                           possibleCount++;
                           break;
                        }
                    }
                }
            }
        }   
    }


    //combine possible + othello board (add 1, and -1 to missing spaces) (0 and 7 are already placed)
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(othelloBoard[i][j] == 1){
                *(possiblePositionBoard[i] + j) = 1;
            }
            else if(othelloBoard[i][j] == -1){
                *(possiblePositionBoard[i] + j) = -1;
            }
        }
    }

    return possibleCount;
}


void putStone(stone currentStoneColor, int othelloBoard[8][8], int userCoordinateX, int userCoordinateY){
    
    int curX, curY, curVal;
    
    //stone of currentStoneColor placed at userCoordinateX and userCoordinateY
    //explore all 8 directions from the placed Stone.
    int incrementCount;
    for(int k = 0; k < 8; k++){
        curX = userCoordinateX + dx[k];
        curY = userCoordinateY + dy[k];
        incrementCount = 1;
        if((curX < 0 || curX >= 8) || (curY < 0 || curY >= 8)){
            continue;
        }
        curVal = othelloBoard[curX][curY];
        if(curVal == currentStoneColor || curVal == 0) {
            continue;
        }
        
        //if it's a different colored stone, go to that direction
        while(1){
            curX = curX + dx[k];
            curY = curY + dy[k];
            incrementCount++;
            if((curX < 0 || curX >= 8) || (curY < 0 || curY >= 8)){
                break;
            }
            curVal = othelloBoard[curX][curY];
            if(curVal == 0){//means empty
                break;
            }
            else if(curVal == currentStoneColor){
                for(int i = 0; i < incrementCount; i++){
                    //othelloBoard[userCoordinateX + i * (dx[k])][userCoordinateY + i * (dy[k])] = currentStoneColor;
                    *(othelloBoard[userCoordinateX + i * (dx[k])] + userCoordinateY + i * (dy[k])) = currentStoneColor;
                }
                break;
            }
        }
    }
    return;
}
