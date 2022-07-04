//
// Created by Casper Wong on 6/19/22.
//

#include <stdbool.h>

#ifndef CHESS_DATASTRUCTS_H
#define CHESS_DATASTRUCTS_H

#define num_pieceTypes 14

enum EPieceType {  // [0, 14)
    whitePawns, whiteKnights, whiteBishops, whiteRooks, 
    whiteQueens, whiteKing, whiteAll, 

    blackPawns, blackKnights, blackBishops, blackRooks,
    blackQueens, blackKing, blackAll
};

enum enumSquare {  // [0, 64)
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
};  // Ordered in same format as a FEN bitboard

struct move_info {
    enum enumSquare from;   // index of origin square
    enum enumSquare to;     // index of destination square
    enum EPieceType piece;  // bitboard to manipulate
};
typedef struct move_info *move;

struct FEN_info{
    uint64_t *BBoard;
    bool whiteToMove;
    char *castling;
    char *enPassant;
    int halfMove;
    int fullMove;
};
typedef struct FEN_info *FEN;

/**
 * Bit masks that determine whether pieces are in specific ranks / files
 */
uint64_t not_a_file;
uint64_t not_ab_file;
uint64_t not_h_file;
uint64_t not_hg_file;

const int LS1Bindex64[64];  // Used for efficient bit-scanning

#endif //CHESS_DATASTRUCTS_H