//
// Created by Casper Wong on 6/19/22.
//

#ifndef CHESS_DEV_TOOLS_H
#define CHESS_DEV_TOOLS_H


/************************
DEBUGGING AND PRINT TOOLS
************************/

/**
 * Prints the board with locations of a single piece type
 * @param currBoard integer that represents the locations of a single piece type
 */
void render_single(int64_t bb);

/**
 * Prints board with a1 at bottom left and h8 at top right
 * @param BBoard
 */
void render_all(uint64_t *BBoard);



/***************************
PYTHON AND LICHESS API TOOLS
***************************/

/**
 * Converts board_fen string into bitboard array. Uses Little-Endian Rank-File Mapping
 * @param board_fen
 * @return bitboards corresponding to enum EPieceType
 * @cite: https://www.chessprogramming.org/Square_Mapping_Considerations#Little-Endian_Rank-File_Mapping
 */
uint64_t *fen2bit(char *board_fen);

/**
 * Tests if strings passed in from Python are successfully parsed
 * @param name A string - should be passed in by Python (ie. FEN string).
 * @return Returns a char for Python API to print
 */
char *greeting(char *name);

#endif //CHESS_DEV_TOOLS_H
