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
 * Class that represents the battleship board game's board
 * xSize is the number of rows, ySize is the number of columns
 * present in the game, default 10
 * */
class Board {

public:
	// Global variables
    int size = 3;
    bool tied = false;
    char lastPlayer = 'x';
    int h = 500, v = 500;//size of window
    int CantorEnum = 0;//used to uniquely identify bigboard squares
    int validX = -1;//the valid big board position that can be played on, 
    int validY = -1;// ^^
    bool wonGame = false;
    int lx = 0;
    int ly = 0;
    //Bigger Board, [rows][cols]
    std::vector<std::vector<int>> bigBoard;

    //Smaller board
    std::vector<std::vector<int>> smallBoard;

    //win detector, each larger box has these
    std::vector<int> bigRow;
    std::vector<int> bigCol;
    std::vector<int> bigDiag;
    std::vector<int> bigAdiag;
    std::vector<std::vector<int>> rows;//(When player wins along rows)
    std::vector<std::vector<int>> col;//(When player wins along columns)
    std::unordered_map<int, int> diag = {};//(When player wins along diagonal)
    std::unordered_map<int, int> adiag = {};//(When player wins along opposite diagonal)

    //player turn detector
    bool player_one_turn = true;

    Board(int s) { 
        size = s;
        bigBoard = (std::vector< std::vector <int> > (size, std::vector <int>(size, 0)));
        smallBoard = std::vector< std::vector <int> > (size*size, std::vector <int>(size*size, 0));
        bigRow = std::vector<int> (size,0);
        bigCol = std::vector<int> (size,0);
        bigDiag = std::vector<int> (size,0);
        bigAdiag = std::vector<int> (size,0);
        rows = std::vector< std::vector <int> > (size * size, std::vector <int>(size, 0));
        col = std::vector< std::vector <int> > (size * size, std::vector <int>(size, 0));
        CantorEnum = -10;
    }

    Board() {}

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
        col = b1.col;
        rows = b1.rows;
        bigRow = b1.bigRow;
        bigCol = b1.bigCol;
        diag = b1.diag;
        adiag = b1.adiag;
    }

    

    /*
    * Check to see if the last move the player played was a valid move
    * (i.e., follows the rules of ultimate tic tac toe)
    *
    * Returns true if players turn ended successfully, false if otherwise
    * NOTE: mutates the playing board
    * PARAM: int x : x coord of move (smaller board wise)
    * PARAM: int y : y coord of move (smaller board wise)
    */
    bool validMove(int x, int y) {

        //See if the position is already filled by either player 
        // or the smaller board is already won, 
        // in rules of ultimate tic tac toe, return
        if (smallBoard[y][x] != 0 || bigBoard[y / size][x / size] != 0) {
            return false;
        }

        int a = validX;
        int b = validY;
        //See if the user can play anywhere
        if (validX == -1 || validY == -1) {
            validX = x / size;
            validY = y / size;
        }
        //See if the user played a valid move for forcing
        if (validX != x / size || validY != y / size) {
            validX = a;
            validY = b;
            return false;
        }

        return true;
    }

    void makeMove(int x, int y) {
        //Valid move played, fill box
        //Player one is 1, player two is -1
        validX = x % size;
        validY = y % size;
        smallBoard[y][x] = player_one_turn ? 1 : -1;
        lx = x;
        ly = y;
        
    }

    void updateScores(int x, int y) {
        //Find the row and colum of the correct big board square
        //Cantor's enumeration to map a unique number to the coord pairs of
        // The big board squares
        CantorEnum = ((x / size + y / size) * (x / size + y / size + 1) / 2) + y / size;
        rows[y][x / size] += player_one_turn ? 1 : -1;
        col[x][y / size] += player_one_turn ? 1 : -1;

        //Check diagonals
        if (x % size == y % size) {
            diag[CantorEnum] += player_one_turn ? 1 : -1;
            //Can be both a diagonal and anti-diagonal placement
            if (x % size == 1) {
                adiag[CantorEnum] += player_one_turn ? 1 : -1;
            }
        }
        else if ((x + y + (x) / size + y / size) % 2 == 0 && x % size != y % size) {
            //Wasn't a diagonal, but an anti-diagonal only
            adiag[CantorEnum] += player_one_turn ? 1 : -1;
        }
    }

    bool checkTied() {
        if (wonGame){ return false; }
        //loop through each of the larger boards and if one isn't won, then
        //it can be played in
        //if each one is filled, then the game cannot go on
        int sum = 0;
        for (int x = 0; x < size; ++x) {
            for (int y = 0; y < size; ++y) {
                sum += bigBoard[y][x];
            }
        }
        return sum == size * size - 1;
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

    void updateBigBoard(int x, int y) {
        //check to see if big board is won, if yes, send win message
            //update bigBoard
        bigBoard[y / size][x / size] = player_one_turn ? 1 : -1;
        bigRow[y / size] += player_one_turn ? 1 : -1;
        bigCol[x / size] += player_one_turn ? 1 : -1;
        if (x / size == y / size) {
            bigDiag[x / size] += player_one_turn ? 1 : -1;
        }
        else if (x / size + y / size + 1 == size) {
            bigAdiag[x / size] += player_one_turn ? 1 : -1;
        }
    }

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
    * Calculates the result of the player's move, then calls checkwin to see
    * if there was a winning move
    *
    * RETURN: void
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