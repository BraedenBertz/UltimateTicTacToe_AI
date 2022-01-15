#pragma once
#ifndef Board_MAIN_H
#define Board_MAIN_H

#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <iostream>
#include <gdiplus.h>
#include <unordered_map>


/*
 * Class that represents the tic tac toe board game's board
 */
class Board {

public:
    // Global variables
    int size = 3;//size of the boards
    bool tied = false;//if gamestate is tied
    char lastPlayer = 'x';//last player who made a move
    int h = 500, v = 500;//size of window
    int CantorEnum = 0;//used to uniquely identify bigboard squares
    int validX = -1;//the valid big board position that can be played on, 
    int validY = -1;// ^^
    bool wonGame = false;//if gamestate has a winner
    int lx = 0;//last played x position
    int ly = 0;//last played y position

    std::vector<std::vector<int>> bigBoard;//Big board of utt, 3 in a row here means game is won
    std::vector<std::vector<int>> smallBoard;//Small boards of utt, 3 in a row here means 1 in a row on bigBoard

    //Big Board win detectors
    std::vector<int> bigRow;//Win in row
    std::vector<int> bigRowCounter;//Counts moves along the specified row, max of 3 for any entry
    std::vector<int> bigCol;//Win in column
    std::vector<int> bigColCounter;//Counts moves along the specified column, max of 3 for any entry
    std::vector<int> bigDiag;//in win diagonal 
    int bigDiagCounter;//Counts moves along the specified diagonal, max of 3 for any entry
    std::vector<int> bigAdiag;//win in anti-diagonal
    int bigAdiagCounter;//Counts moves along the specified anti-diagonal, max of 3 for any entry

    //Small board win detectors
    std::vector<std::vector<int>> rows;//(When player wins along rows)
    std::vector<std::vector<int>> rowsCounter;//Counts moves along the specified row, max of 3 for any entry
    std::vector<std::vector<int>> col;//(When player wins along columns)
    std::vector<std::vector<int>> colCounter;//Counts moves along the specified column, max of 3 for any entry
    std::unordered_map<int, int> diag = {};//(When player wins along diagonal)
    std::unordered_map<int, int> diagCounter = {};//Counts moves along the specified diagonal, max of 3 for any entry
    std::unordered_map<int, int> adiag = {};//(When player wins along opposite diagonal)
    std::unordered_map<int, int> adiagCounter = {};//Counts moves along the specified anti-diagonal, max of 3 for any entry

    //player turn detector
    bool player_one_turn = true;

    /*
    * Create an ultimate tic tac toe Board with the given size 
    * 
    * @param: int s: size of the utt board, default is 3
    */
    Board(int s) {
        size = s;
        bigBoard = (std::vector< std::vector <int> >(size, std::vector <int>(size, 0)));
        smallBoard = std::vector< std::vector <int> >(size * size, std::vector <int>(size * size, 0));
        bigRow = std::vector<int>(size, 0);
        bigCol = std::vector<int>(size, 0);
        bigDiag = std::vector<int>(size, 0);
        bigAdiag = std::vector<int>(size, 0);
        rows = std::vector< std::vector <int> >(size * size, std::vector <int>(size, 0));
        col = std::vector< std::vector <int> >(size * size, std::vector <int>(size, 0));
        rowsCounter = std::vector< std::vector <int> >(size * size, std::vector <int>(size, 0));
        colCounter = std::vector< std::vector <int> >(size * size, std::vector <int>(size, 0));
        bigAdiagCounter = 0;
        bigDiagCounter = 0;
        bigRowCounter = std::vector<int>(size, 0);
        bigColCounter = std::vector<int>(size, 0);
        CantorEnum = -10;
    }

    /*
    * default constructor
    */
    Board() {}

    /*
    * copy constructor
    * 
    * @param: b1: Board object we will copy from
    */
    Board(const Board& b1) {
        CantorEnum = b1.CantorEnum;
        validX = b1.validX;
        validY = b1.validY;
        wonGame = b1.wonGame;
        player_one_turn = b1.player_one_turn;
        tied = b1.tied;
        lx = b1.lx;
        ly = b1.ly;

        bigBoard = (b1.bigBoard);
        smallBoard = b1.smallBoard;

        bigAdiag = b1.bigAdiag;
        bigDiag = b1.bigDiag;
        bigRow = b1.bigRow;
        bigCol = b1.bigCol;

        diag = b1.diag;
        adiag = b1.adiag;
        rows = b1.rows;
        col = b1.col;

        colCounter = b1.colCounter;
        rowsCounter = b1.rowsCounter;
        diagCounter = b1.diagCounter;
        adiagCounter = b1.adiagCounter;
        bigAdiagCounter = b1.bigAdiagCounter;
        bigColCounter = b1.bigColCounter;
        bigRowCounter = b1.bigRowCounter;
        bigDiagCounter = b1.bigDiagCounter;

    }

    /*
    * Check to see if the last move the player played was a valid move
    * (i.e., follows the rules of ultimate tic tac toe)
    *
    * Returns true if players turn ended successfully, false if otherwise
    * 
    * PARAM: int x : x coord of move (smaller board wise)
    * PARAM: int y : y coord of move (smaller board wise)
    */
    bool validMove(int x, int y) {
        //See if the position is already filled by either player 
        // or the smaller board is already won, 
        // in rules of ultimate tic tac toe, return
        
        if (smallBoard[y][x] != 0 || bigBoard[y / size][x / size] != 0) { return false; }
        else if ((validX == -1 || validY == -1)) { return true; } 
        else if (validX != x / size || validY != y / size) { return false; }
        return true;
    }

    /*
    * Make a move on the smallBoard, then change the appropriate variables to reflect turn was done
    * 
    * @param: int x: the x position of smallboard the move will be done
    * @param: int y: the y position of smallboard the move will be done
    */
    void makeMove(int x, int y) {
        //Valid move played, fill box
        //Player one is 1, player two is -1
        validX = x % size;
        validY = y % size;
        smallBoard[y][x] = player_one_turn ? 1 : -1;
        lx = x;
        ly = y;
    }

    /*
    * update the scores of the board
    * 
    * @param: int x: last played x position
    * @param: int y: last played y position
    */
    void updateScores(int x, int y) {
        //Find the row and colum of the correct big board square
        //Cantor's enumeration to map a unique number to the coord pairs of
        // The big board squares
        CantorEnum = ((x / size + y / size) * (x / size + y / size + 1) / 2) + y / size;
        rows[y][x / size] += player_one_turn ? 1 : -1;
        col[x][y / size] += player_one_turn ? 1 : -1;
        colCounter[x][y / size] += 1;
        rowsCounter[y][x / size] += 1;

        //Check diagonals
        if (x % size == y % size) {
            diag[CantorEnum] += player_one_turn ? 1 : -1;
            diagCounter[CantorEnum] += 1;
            //Can be both a diagonal and anti-diagonal placement
            if (x % size == 1) {
                adiag[CantorEnum] += player_one_turn ? 1 : -1;
                adiagCounter[CantorEnum] += 1;
            }
        }
        else if ((x + y + (x) / size + y / size) % 2 == 0 && x % size != y % size) {
            //Wasn't a diagonal, but an anti-diagonal only
            adiag[CantorEnum] += player_one_turn ? 1 : -1;
            adiagCounter[CantorEnum] += 1;
        }
    }

    /*
    * Check to see if current gamestate is a tie
    * 
    * @return: true if tied, false if there are still valid moves available
    */
    bool checkTied() {
        if (wonGame) { return false; }
        //loop through each of the larger boards and if one isn't won, then
        //it can be played in if there is a 0 cell
        //if each one is filled, then the game cannot go on
        int sum = 0;
        for (int x = 0; x < size; ++x) {
            for (int y = 0; y < size; ++y) {
                //possibly tied
                if (bigBoard[y][x] == 0) {
                    for (auto j = 0; j < size; j++) {
                        for (auto i = 0; i < size; i++) {
                            //bigBoard at [y][x] is not won, see if it is full, otw, you can play in that
                            if (smallBoard[y * size + j][x * size + i] == 0) { return false; }
                        }
                    }
                }
            }
        }
        return true;
    }

    /*
        Check to see if there was n in a row (col, row, diag, adiag) on the smaller board
        and then if there is coordinate on the bigger board and see
        if there is n in a row from the smaller board to bigger board
        If there is, then whoever has the n in a row in the bigger board
        is the winner of the overall game

        Return: true if there was a win on the small board, false if otherwise
        Param: int x : the x coordinate of the move (smaller board wise)
        Param: int y : ^^ but y coord
    */
    bool checkWinSmallBoard(int x, int y) {
        //Check to see if the small board is won, if so, coordinate on big board    
        bool wonSmallBoard =
            rows[y][x / size] == size || rows[y][x / size] == -size
            || col[x][y / size] == size || col[x][y / size] == -size
            || diag[CantorEnum] == size || diag[CantorEnum] == -size
            || adiag[CantorEnum] == size || adiag[CantorEnum] == -size;
        return wonSmallBoard;
    }

    /*
    * update the bigBoard to reflect the position of the smallboards
    * 
    * @param: int x: the x position of smallboard the move will be done
    * @param: int y: the y position of smallboard the move will be done
    */
    void updateBigBoard(int x, int y) {
        //check to see if big board is won, if yes, send win message
            //update bigBoard
        bigBoard[y / size][x / size] = player_one_turn ? 1 : -1;
        bigRow[y / size] += player_one_turn ? 1 : -1;
        bigCol[x / size] += player_one_turn ? 1 : -1;
        bigRowCounter[y / size] += 1;
        bigColCounter[x / size] += 1;
        if (x / size == y / size) {
            bigDiag[x / size] += player_one_turn ? 1 : -1;
            bigDiagCounter++;
            if (x / size + y / size + 1 == size) {
                bigAdiag[x / size] += player_one_turn ? 1 : -1;
                bigAdiagCounter++;
            }
        }
        else if (x / size + y / size + 1 == size) {
            bigAdiag[x / size] += player_one_turn ? 1 : -1;
            bigAdiagCounter++;
        }
    }

    /*
    * Check to see if the bigboard is won in any of the directions
    * 
    * @return: true if there is a win on the bigboard, false if otherwise
    */
    bool checkWinBigBoard() {

        //If the sum of sum or adiag = n, then there was an
        // n row in the diagonal or anti-diagonal of the 
        // same type (either X or O)
        int sum = 0;
        int asum = 0;
        for (int x = 0; x < size; x++) {
            sum += bigDiag[x];
            asum += bigAdiag[x];
        }

        return (bigRow[ly / size] == size || bigRow[ly / size] == -size ||
            bigCol[lx / size] == size || bigCol[lx / size] == -size ||
            sum == size || asum == size ||
            sum == -size || asum == -size);
    }

    /*
    * Goes through the logic of making a turn in utt
    *
    * @param: int x: the x position of smallboard the move will be done
    * @param: int y: the y position of smallboard the move will be done
    * @return: true if move was made, false if otherwise
    */
    bool makeTurn(int x, int y) {

        //Player didn't play a legal move, retry
        if (!validMove(x, y)) { return false; }

        //Player's move is valid, so do move
        makeMove(x, y);

        updateScores(x, y);

        if (checkWinSmallBoard(x, y)) {
            updateBigBoard(x, y);
            wonGame = checkWinBigBoard();
        }

        tied = checkTied();

        //if bigboard at validX,validY is filled in, 
        // then the player can play anywhere not filled in
        if (bigBoard[validY][validX] != 0) {
            validX = -1;
            validY = -1;
        }
        lastPlayer = player_one_turn ? 'x' : 'o';
        player_one_turn = !player_one_turn;

        return true;
    }

};

#endif
