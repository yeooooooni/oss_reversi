#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "othutility.h"


void refreshInputWindowToDefault(WINDOW* inputWin, stone currentStoneColor);
void displayNoPossiblePositions(WINDOW* inputWin, stone currentStoneColor);
void calculateScore(int othelloBoard[8][8], int* countBlack, int* countWhite, int* countEmpty);
void updateScoreBoard(WINDOW* scoreWin, int countBlack, int countWhite); //maybe update this later to consider countwhites and countpossible too?? we'll see
void showPossiblePositions(int possiblePosition[8][8], stone currentStoneColor, WINDOW* gridbox[8][8]);
void showOthelloBoard(int othelloBoard[8][8], WINDOW* gridbox[8][8]);//reloads, shows only b and w stones
void receiveUserInput(WINDOW* inputWin, int possiblePosition[8][8], stone currentStoneColor, int* userCoordRow, int* userCoordCol);

int main(int argc, char* argv[]){

    //add game start screen with ascii art  
    //input name do it in console and then start
    // char name[BUFSIZ];
    // printf("Enter your player name: ");
    // scanf("%s", name);
    //assign stone (and perhaps turnID beforehand)



    //declare and initialize matrix for othelloboard
    int othelloBoard[8][8]; // = {0}
    initializeArraytoAllZeros(othelloBoard);


    //initial setup should be 2 black and 2 white stones 
    othelloBoard[3][3] = -1;
    othelloBoard[3][4] = 1;
    othelloBoard[4][3] = 1;
    othelloBoard[4][4] = -1;

    //declare and initialize matrix for possible options 
    int possible[8][8];
    initializeArraytoAllZeros(possible);


    int turn = -1; //-1 means black, 1 means white. 
    int countBlack = 2;
    int countWhite = 2;
    int countEmpty, possiblePositions;
    int userCoordRow, userCoordCol;

    int currentStoneColor = BLACK;


    player p1;
    strcpy(p1.name,"jun");
    p1.stone = BLACK;
    player p2;
    strcpy(p1.name,"ethan raj");
    p1.stone = WHITE;

    /////////////////////////////////////////////////////////////////////////// array setup for backend 


    initscr();
    //clear screen
    //ascii art welcome + user name 

    // printw("welcomme %s!\n", name);
    // printw("press any key to continue...");
    // getch();
    // clear();

    start_color();
    if(!has_colors()){
        printw("color is not supported in this terminal, try on a different terminal");
        exit(1);
    }
    
    init_pair(1,COLOR_BLACK, COLOR_WHITE); //bg white for white stones 
    init_pair(2,COLOR_WHITE, COLOR_BLACK); //bg black for black stones 
    init_pair(3,COLOR_WHITE, COLOR_YELLOW); //bg yellow for avaiable positions 
    init_pair(4,COLOR_WHITE, COLOR_GREEN);
    
    bkgd(COLOR_PAIR(4));
    
    int yBeg, xBeg;
    int yMax, xMax;
    int gridHeight, gridWidth;

    getmaxyx(stdscr, yMax, xMax);
    int rowInc = (yMax - 5) / 9;
    int colInc =  xMax / 10;
    
    gridHeight = rowInc;
    gridWidth= gridHeight * 2;

    //printw("row inc, colInc: %d, %d ", rowInc, colInc);
    refresh();

    //construct gridbox for the othello board.     
    WINDOW* gridbox[8][8];

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            //h, w, start y(row), start x(col)
            gridbox[i][j] = newwin(gridHeight, gridWidth, rowInc * (i+1),  colInc * (j+1));
            wbkgd(gridbox[i][j], COLOR_PAIR(4)); // same as bg initially
            box(gridbox[i][j], 0, 0);
            wrefresh(gridbox[i][j]);
        }
    }

    wbkgd(gridbox[3][3], COLOR_PAIR(2));
    //wclear(gridbox[3][3]);
    wrefresh(gridbox[3][3]);

    wbkgd(gridbox[3][4], COLOR_PAIR(1));
    //wclear(gridbox[3][4]);
    wrefresh(gridbox[3][4]);

    wbkgd(gridbox[4][3], COLOR_PAIR(1));
    //wclear(gridbox[4][3]);
    wrefresh(gridbox[4][3]);

    wbkgd(gridbox[4][4], COLOR_PAIR(2));
    //wclear(gridbox[4][4]);
    wrefresh(gridbox[4][4]);

    //test for possible grids
    // wbkgd(gridbox[4][5], COLOR_PAIR(3));
    // //wclear(gridbox[4][4]);
    // wrefresh(gridbox[4][5]);

    //label rows and columns 
    //rows
    for(int j = 0; j < 8; j++){
        move((rowInc) * (j + 1) + (gridHeight / 2 - 1), colInc - 2);
        //printw("%c", (48 + j + 1));
        addch((48 + j + 1));
    }
    //cols
    for(int i = 0; i < 8; i++){
        move(rowInc - 1, (colInc) * (i + 1) + (gridWidth/2));
        //printw("%c", (48 + i + 1));
        addch((48 + i + 1));
    }

    refresh();


    
    
    //construct window for displaying score
    WINDOW* scoreWin = newwin(gridHeight * 1.5 + 1, xMax/2 - (3 * gridWidth), yMax - 2 * rowInc, colInc);

    /*
    box(scoreWin, 0, 0);
    //wclear(scoreWin);
    wbkgd(scoreWin, COLOR_PAIR(1));
    wmove(scoreWin, 1, 1);
    wprintw(scoreWin, "Score Board");
    int rowMaxScoreWin, colMaxScoreWin;
    getmaxyx(scoreWin, rowMaxScoreWin, colMaxScoreWin);
    wmove(scoreWin, rowMaxScoreWin/2, colMaxScoreWin/2 - 7); //7 is like half the strlen of the printed str
    wprintw(scoreWin, "Player1(Black): %d", countBlack);
    wmove(scoreWin, rowMaxScoreWin/2 + 1, colMaxScoreWin/2 - 7);
    wprintw(scoreWin, "Player2(White): %d", countWhite);
    wrefresh(scoreWin);
    */
    updateScoreBoard(scoreWin, countBlack, countWhite);


    //construct window for receiving input. 
    WINDOW* inputWin = newwin(gridHeight * 1.5 + 1, xMax/2 - (3 * gridWidth) + 2, 
                                yMax - 2 * rowInc,  xMax/2 - (3 * gridWidth) + gridWidth + colInc);

    /*
    box(inputWin, 0, 0);
    //wclear(inputWin);
    wbkgd(inputWin, COLOR_PAIR(1));
    wmove(inputWin, 1, 1);
    wprintw(inputWin, "Input Window (Row,Column)");
    wmove(inputWin, 2, 1);
    wprintw(inputWin, "BLACK's TURN");
    int rowMaxinputWin, colMaxinputWin;
    getmaxyx(inputWin, rowMaxinputWin, colMaxinputWin);
    //if my turn -> display enter coordinate 
    wmove(inputWin, rowMaxinputWin/2, 1); 
    wprintw(inputWin, "Enter Coordinate: "); //18 chars
    */
    int rowMaxinputWin, colMaxinputWin;
    getmaxyx(inputWin, rowMaxinputWin, colMaxinputWin);                         
    
    refreshInputWindowToDefault(inputWin , currentStoneColor);


    // char inputCoordinate[BUFSIZ];
    // mvwgetstr(inputWin, rowMaxinputWin/2, 18 + 1, inputCoordinate);

    // if(strchr(inputCoordinate, ',') == NULL){
    //     wmove(inputWin, rowMaxinputWin/2 + 1, 1); 
    //     wprintw(inputWin, "Wrong Input, Try Again"); 
    //     wrefresh(inputWin);
    // }
    // else{
    //     char* userCoordR = strtok(inputCoordinate, ",");
    //     char* userCoordC = strtok(NULL, ",");
    //     wmove(inputWin, rowMaxinputWin/2 + 1, 1); 
    //     wprintw(inputWin, "You Entered: (%s, %s)", userCoordR, userCoordC); //13 chars
    //     int userCoordRInt = (int)userCoordR[0] - 48; // 48 == '0'
    //     int userCoordCInt = (int)userCoordC[0] - 48;
    //     wprintw(inputWin, "  : test (%d, %d)", userCoordRInt, userCoordCInt);
    // }
    // // add more error cases if time allows (with strtok, separators, ex( 4,3 5,   2 // isdigit() etc....))
    
    // //if not my turn -> display oponent's turn 
    // wrefresh(inputWin);



    // //show possi
    // findPossible(currentStoneColor, othelloBoard, possible);
    // showPossiblePositions(possible, currentStoneColor, gridbox);

    // printw("aldfkjadfkjadkjlkadfjlk");
    // getch(); //after this it should turn back to w/o yellow grids

    // showOthelloBoard(othelloBoard, gridbox);
    // /**/


    while(1){//othello game loop
        refreshInputWindowToDefault(inputWin, currentStoneColor);

        possiblePositions = findPossible(currentStoneColor, othelloBoard, possible);
        if(possiblePositions == 0){
            displayNoPossiblePositions(inputWin, currentStoneColor);
            currentStoneColor = -currentStoneColor;
            continue;    
        }
        showPossiblePositions(possible, currentStoneColor, gridbox);
        //receive input (test for right input)
        receiveUserInput(inputWin, possible, currentStoneColor, 
            &userCoordRow, &userCoordCol);

        //with right input put stone on the array (and flip stones)
        putStone(currentStoneColor, othelloBoard, userCoordRow, userCoordCol);

        //display updated othelloboard
        showOthelloBoard(othelloBoard, gridbox);

        //calculate number of black, white, and whitespace
        calculateScore(othelloBoard, &countBlack, &countWhite, &countEmpty);

        //display updated score (calculate whether game is done or not)
        updateScoreBoard(scoreWin, countBlack, countWhite);

        //end of GAME requirement
        if(countEmpty == 0){
            //clear screen
            //go outside of loop and print endOfGame Message with results 
            break;
        }

        //initialize possible array
        initializeArraytoAllZeros(possible);

        //press any key to pass the turn
        getch();

        //pass turn
        currentStoneColor = -currentStoneColor;
    }

    

    //end of screen.
    mvprintw(yMax/2, xMax/2 - 5, "END OF GAME");
    if(countBlack == countWhite){
        mvprintw(yMax/2 + 1, xMax/2 - 5, "IT\'S A TIE");
    }
    else if(countBlack > countWhite){
        mvprintw(yMax/2 + 1, xMax/2 - 9, "BLACK IS THE WINNER");
    }
    else if(countBlack < countWhite){
        mvprintw(yMax/2 + 1, xMax/2 - 9, "WHITE IS THE WINNER");
    }

    mvprintw(yMax/2 + 2, xMax/2 - 12, "PRESS ANY KEY TO END GAME");

    getch();
    endwin();

    // char yesOrNo;
    // printf("Do you want to play again?\n");
    // scanf("%c", &yesOrNo);
    // if(yesOrNo == 'n') {
    //     exit(0);
    // }

    return 0;
}


// maybe need add case where it skips over turns 
void refreshInputWindowToDefault(WINDOW* inputWin, stone currentStoneColor){
    
    //this is the default state for inputWin
    wclear(inputWin);
    box(inputWin, 0, 0);
    wbkgd(inputWin, COLOR_PAIR(1));
    wmove(inputWin, 1, 1);
    wprintw(inputWin, "Input Window (Row,Column)");
    wmove(inputWin, 2, 1);
    if(currentStoneColor == BLACK){ //maybe change attributes to blink or sth...?? we'll see 
        wprintw(inputWin, "BLACK's TURN");
    }
    else if(currentStoneColor == WHITE){
        wprintw(inputWin, "WHITE's TURN");
    }

    int rowMaxinputWin, colMaxinputWin;
    getmaxyx(inputWin, rowMaxinputWin, colMaxinputWin);
    //if my turn -> display enter coordinate 
    wmove(inputWin, rowMaxinputWin/2, 1); 
    wprintw(inputWin, "Enter Coordinate: "); //18 chars
    wrefresh(inputWin);
    return;
}

void displayNoPossiblePositions(WINDOW* inputWin, stone currentStoneColor){
    //this is the default state for inputWin
    wclear(inputWin);
    box(inputWin, 0, 0);
    wbkgd(inputWin, COLOR_PAIR(1));
    wmove(inputWin, 1, 1);
    wprintw(inputWin, "Input Window (Row,Column)");
    wmove(inputWin, 2, 1);
    if(currentStoneColor == BLACK){ //maybe change attributes to blink or sth...?? we'll see 
        wprintw(inputWin, "BLACK's TURN");
    }
    else if(currentStoneColor == WHITE){
        wprintw(inputWin, "WHITE's TURN");
    }

    int rowMaxinputWin, colMaxinputWin;
    getmaxyx(inputWin, rowMaxinputWin, colMaxinputWin);
    //if my turn -> display enter coordinate 
    wmove(inputWin, rowMaxinputWin/2, 1); 
    wprintw(inputWin, "NO POSSIBLE POSITIONS FOR THIS TURN");
    
    wmove(inputWin, rowMaxinputWin/2 + 1, 1); 
    wprintw(inputWin, "PRESS ANY KEY TO CONTINUE");

    wrefresh(inputWin);
    getch();
    return;
}

void calculateScore(int othelloBoard[8][8], int* countBlack, int* countWhite, int* countEmpty){
    int emptyCells = 0;
    int BlackCells = 0;
    int WhiteCells = 0;

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(othelloBoard[i][j] == 0) {
               emptyCells++;
            }  
            else if(othelloBoard[i][j] == -1) {
               BlackCells++;
            } 
            else if(othelloBoard[i][j] == 1) {
               WhiteCells++;
            }  
        }
    }

    *countEmpty = emptyCells;
    *countBlack = BlackCells;
    *countWhite = WhiteCells;

    return;
}


void updateScoreBoard(WINDOW* scoreWin, int countBlack, int countWhite){
    
    wclear(scoreWin);
    box(scoreWin, 0, 0);
    wbkgd(scoreWin, COLOR_PAIR(1));
    wmove(scoreWin, 1, 1);
    wprintw(scoreWin, "Score Board");
    int rowMaxScoreWin, colMaxScoreWin;
    getmaxyx(scoreWin, rowMaxScoreWin, colMaxScoreWin);
    wmove(scoreWin, rowMaxScoreWin/2, colMaxScoreWin/2 - 7); //7 is like half the strlen of the printed str
    wprintw(scoreWin, "Player1(Black): %d", countBlack);
    wmove(scoreWin, rowMaxScoreWin/2 + 1, colMaxScoreWin/2 - 7);
    wprintw(scoreWin, "Player2(White): %d", countWhite);
    wrefresh(scoreWin);
}


void showPossiblePositions(int possiblePosition[8][8], stone currentStoneColor, WINDOW* gridbox[8][8]){

    int charBoard[8][8];
    initializeArraytoAllZeros(charBoard);

    //replicate int array to char array 
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(possiblePosition[i][j] == 0){
                charBoard[i][j] = ' ';
            }
            else if(possiblePosition[i][j] == 7){
                charBoard[i][j] = 'P';
            }
            else if(possiblePosition[i][j] == 1){
                charBoard[i][j] = 'W';
            }
            else if(possiblePosition[i][j] == -1){
                charBoard[i][j] = 'B';
            }
        }
    }
    

    // printf("Possible Positions for ");
    // if(currentStoneColor == BLACK){
    //     printf("BLACK STONES\n");
    // }
    // else{
    //     printf("WHITE STONES\n");
    // }
    

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(charBoard[i][j] == 'P') {
                wbkgd(*(gridbox[i] + j), COLOR_PAIR(3)); // 3 is for yellow (possible positions)
                // box(gridbox[i][j], 0, 0);
                box(*(gridbox[i] + j), 0, 0);
                // wrefresh(gridbox[i][j]);
                wrefresh(*(gridbox[i] + j));
            }  
        }
    }

    return;
}

//reloads, shows only b and w stones
void showOthelloBoard(int othelloBoard[8][8], WINDOW* gridbox[8][8]){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(othelloBoard[i][j] == 0) {
                wbkgd(*(gridbox[i] + j), COLOR_PAIR(4)); // 4 is for Default (for empty)
                box(*(gridbox[i] + j), 0, 0);
                wrefresh(*(gridbox[i] + j));
            }   
            else if(othelloBoard[i][j] == -1) {
                wbkgd(*(gridbox[i] + j), COLOR_PAIR(2)); // 2 is for Black
                box(*(gridbox[i] + j), 0, 0);
                wrefresh(*(gridbox[i] + j));
            }
            else if(othelloBoard[i][j] == 1) {
                wbkgd(*(gridbox[i] + j), COLOR_PAIR(1)); // 1 is for White
                box(*(gridbox[i] + j), 0, 0);
                wrefresh(*(gridbox[i] + j));
            }   
        }
    }
}



void receiveUserInput(WINDOW* inputWin, int possiblePosition[8][8], 
        stone currentStoneColor, int* userCoordRow, int* userCoordCol){
    
    //receive 
    //test
            //wrong format
            //not possible
            //out of bounds 
    //if not -> repeat
    //if good -> pass value to reference

    //refreshInputWindowToDefault(inputWin , currentStoneColor);
    char* userCoordR;
    char* userCoordC;
    int rowMaxinputWin, colMaxinputWin;
    getmaxyx(inputWin, rowMaxinputWin, colMaxinputWin);     

    char inputCoordinate[BUFSIZ];
    while(1){
        refreshInputWindowToDefault(inputWin, currentStoneColor);
        mvwgetstr(inputWin, rowMaxinputWin/2, 18 + 1, inputCoordinate);

        if(strchr(inputCoordinate, ',') == NULL){
            wmove(inputWin, rowMaxinputWin/2 + 1, 1); 
            wprintw(inputWin, "Wrong Input, Try Again(Press any key to continue)"); 
            wrefresh(inputWin);
            getch();
            continue;
        }
        else{
            userCoordR = strtok(inputCoordinate, ",");
            userCoordC = strtok(NULL, ",");

            if(strlen(userCoordR) != 1 || strlen(userCoordC) != 1){
                wmove(inputWin, rowMaxinputWin/2 + 1, 1); 
                wprintw(inputWin, "Wrong Input, Try Again(Press any key to continue)"); 
                wrefresh(inputWin);
                getch();
                continue;
            }
            //check is digit 
            //if it is digit convert to int. 
            int userCoordRInt = (int)userCoordR[0] - 48; // 48 == '0'
            int userCoordCInt = (int)userCoordC[0] - 48;
            //if it is digit
            //test OOB 
            if(userCoordCInt - 1 < 0 || userCoordCInt - 1 > 7 ||
                userCoordRInt - 1 < 0 || userCoordRInt - 1 > 7
            ){
                wmove(inputWin, rowMaxinputWin/2 + 1, 1); 
                wprintw(inputWin, "Wrong Input, Try Again(Press any key to continue)"); 
                wrefresh(inputWin);
                getch();
                continue;
            }
            //test possible 
            userCoordCInt = userCoordCInt - 1;
            userCoordRInt = userCoordRInt - 1;
            if(possiblePosition[userCoordRInt][userCoordCInt] != 7){
                wmove(inputWin, rowMaxinputWin/2 + 1, 1); 
                wprintw(inputWin, "Wrong Input, Try Again(Press any key to continue)"); 
                wrefresh(inputWin);
                getch();
                continue;
            }
            //if tests passed. 
            *userCoordRow = userCoordRInt;
            *userCoordCol = userCoordCInt;
            wmove(inputWin, rowMaxinputWin/2 + 1, 1); 
            wprintw(inputWin, "You Entered: (%s, %s)", userCoordR, userCoordC); //13 chars
            wmove(inputWin, rowMaxinputWin/2 + 2, 1); 
            wprintw(inputWin, "PRESS ANY KEY TO END TURN");
            wrefresh(inputWin);
            break;
            
        }
        wrefresh(inputWin);
    }
    return;
}