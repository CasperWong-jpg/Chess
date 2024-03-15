//
// Created by Casper Wong on 6/19/22.
//

#ifndef CHESS_BOARD_MANIPULATIONS_H
#define CHESS_BOARD_MANIPULATIONS_H

/**
 * Macro function to vertically flip a square
 */
#define FLIP(sq) ((sq)^56)

/**********************
 * BASIC BIT OPERATIONS
**********************/
/**
 * Finds index of the first piece on bitboard
 * @param bb A single bitboard
 * @return Index [0, 64) of the least significant 1's bit
 * @cite: This function is from https://www.chessprogramming.org/BitScan#De_Bruijn_Multiplication
 */
enum enumSquare bitScanForward(uint64_t bb);

/**
 * Finds index of the last piece on bitboard
 * @param bb A single bitboard
 * @return Index [0, 64) of the most significant 1's bit
 * @cite: This function is from https://www.chessprogramming.org/BitScan#De_Bruijn_Multiplication_2
 */
enum enumSquare bitScanReverse(uint64_t bb);

/**
 * @param bb A single bitboard
 * @return Number of pieces (1 bits) on the bitboard
 * @cite: See Brian Kernighan's way in https://www.chessprogramming.org/Population_Count
 */
int popCount(uint64_t bb);


/********
 * MASKS
********/
/**
 * Generates sliding piece line attack masks given index of a piece
 * @param sq index of
 * @return bitboard with 1's for corresponding line
 * @cite https://www.chessprogramming.org/On_an_empty_Board#By_Calculation_3
 */
uint64_t rankMask(enum enumSquare sq);  // horizontal
uint64_t fileMask(enum enumSquare sq);  // vertical
uint64_t diagonalMask(enum enumSquare sq);
uint64_t antiDiagMask(enum enumSquare sq);

uint64_t northRay(enum enumSquare sq);
uint64_t southRay(enum enumSquare sq);
uint64_t eastRay(enum enumSquare sq);
uint64_t westRay(enum enumSquare sq);
uint64_t northEastRay(enum enumSquare sq);
uint64_t southWestRay(enum enumSquare sq);
uint64_t northWestRay(enum enumSquare sq);
uint64_t southEastRay(enum enumSquare sq);


/*********************
 * BOARD MANIPULATIONS
*********************/
/**
 * Make the move specified by m on the board BBoard
 * @param BBoard
 * @param m
 * @return Nothing. BBoard will be changed
 * From profiling performance, creating copy of bitboard is just as fast as unmaking move
 */
void make_move(uint64_t *BBoard, move m);

#endif //CHESS_BOARD_MANIPULATIONS_H
