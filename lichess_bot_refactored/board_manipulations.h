//
// Created by Casper Wong on 6/19/22.
//

#ifndef CHESS_BOARD_MANIPULATIONS_H
#define CHESS_BOARD_MANIPULATIONS_H

/**
 * Finds index of the first piece on bitboard
 * @param bb A single bitboard
 * @return Index [0, 64) of the least significant 1's bit
 * @cite: This function is from https://www.chessprogramming.org/BitScan
 */
int bitScanForward(uint64_t bb);

/**
 * @param bb A single bitboard
 * @return Number of pieces (1 bits) on the bitboard
 * @cite: See Brian Kernighan's way in https://www.chessprogramming.org/Population_Count
 */
int popCount(uint64_t bb);

// TODO: Implement the following:
bool make_move(uint64_t *BBoard, move m);

#endif //CHESS_BOARD_MANIPULATIONS_H