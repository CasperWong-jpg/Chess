#define num_pieceTypes 14

#ifdef uint64_t
typedef unsigned long long uint64_t;
#endif

#ifdef int64_t
typedef long int64_t;
#endif


enum EPieceType {  // 0 - 13
    whitePawns, whiteKnights, whiteBishops, whiteRooks, 
    whiteQueens, whiteKing, whiteAll, 

    blackPawns, blackKnights, blackBishops, blackRooks,
    blackQueens, blackKing, blackAll
};

enum enumSquare {  // 0 - 63
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8
};

typedef struct move_info *move;

// Takes a square index, and returns its' corresponding attack board.
// NOTE: Also have the mask_knight_attacks function. Delete one of them.
uint64_t mask_knight_attacks(uint64_t bb_pos);

const int LS1Bindex64[64];
uint64_t not_a_file;
uint64_t not_h_file;
uint64_t not_hg_file;
uint64_t not_ab_file;

const uint64_t knight_attack_lookup[64];
