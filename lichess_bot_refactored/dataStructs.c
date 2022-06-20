#include <stdint.h>

struct move_info {
    char move[2];    // origin and destination squares (0 - 63)
    char side;    // side moving white = 0 (whitePawns) or black = 7 (blackPawns)
    char piece;    // see enum EPieceType
};

uint64_t not_a_file = 18374403900871474942ULL;
uint64_t not_h_file = 9187201950435737471ULL;
uint64_t not_hg_file = 4557430888798830399ULL;
uint64_t not_ab_file = 18229723555195321596ULL;

const int LS1Bindex64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
};
