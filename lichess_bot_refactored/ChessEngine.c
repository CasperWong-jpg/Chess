/**
 * TODO:
 *  1. Learn how to use debugger (set up cmakefile and config)
 *  2. Create pseudo-legal move generation for all piece types
 *  3. Do legality checks
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/contracts.h"
#include "dataStructs.h"
#include "dev_tools.h"
#include "board_manipulations.h"

#ifndef DEBUG
#define DEBUG = true;
#endif


/*****************************
 * GENERIC AI AND MOVE HELPERS
*****************************/

/**
 * Evaluates material balance of position boards
 * @return Sum(weighting * (whiteCount - blackCount)) ie. +ve means white advantage
 */
int evaluateMaterial(uint64_t *BBoard, bool whiteToMove) {
    int score = 0;
    int values[] = {100, 300, 300, 500, 900, 20000};  // p, n, b, r, q, k
    for (enum EPieceType i = 0; i < 6; i++) {
        int count = popCount(BBoard[i]) - popCount(BBoard[i + 7]);
        score += values[i] * count;
    }
    if (!whiteToMove) score *= -1;
    return score;
}


/**
 * Checks that there is no friendly piece at specified square
 */
bool noFriendlyPieceAt(uint64_t *BBoard, uint64_t square, bool whiteToMove) {
    uint64_t friendlyBoard = BBoard[whiteAll + !whiteToMove * 7];
    return !(friendlyBoard & square);
}


/******************************
 * SLIDING PIECE MOVE FUNCTIONS
******************************/
/**
 * Checks if a generated sliding piece move is pseudo-legal or not.
 * @param BBoard Pre-move bitboard
 * @param preMove Position of sliding piece prior to move
 * @param postMove Position of sliding piece after generated move
 */
bool isPseudoLegalSlidingMove(uint64_t *BBoard, uint64_t preMove, uint64_t postMove, bool whiteToMove) {
    // Sliding pieces cannot pass through others, and are not cannibals!
    return noFriendlyPieceAt(BBoard, postMove, whiteToMove);
}


/************************
 * ROOK MOVE FUNCTIONS
************************/
/**
 * Generates all (possibly illegal) rook moves
 * @param rook_index index of current rook
 * @return bitboard containing the possible positions that rook can move to
 */
uint64_t getRookMoves(char rook_index) {
    uint64_t rook = 1UL << rook_index;
    return rook ^ (rankMask(rook_index) | fileMask(rook_index));
}


/**
 * Generates all legal moves that rooks can make
 * @param BBoard
 * @param whiteToMove
 * @return An array of move_info pointers that contain all legal rook moves
 */
move *generateMoves_rook(uint64_t *BBoard, bool whiteToMove) {
    // Get rook board of color to move
    uint64_t rooksBoard = BBoard[whiteRooks + !whiteToMove * 7];
#ifdef DEBUG
    printf("Rook board - ");
    render_single(rooksBoard);
#endif
    while (rooksBoard) {
        // For each rook, generate all pseudo-legal moves
        enum enumSquare rookPosition = bitScanForward(rooksBoard);
        uint64_t rookMoves = getRookMoves(rookPosition);
#ifdef DEBUG
        printf("Rook moves - ");
        render_single(rookMoves);
#endif
        while (rookMoves) {
            // For each rook move, check if legal
            enum enumSquare rookMove = bitScanForward(rookMoves);
            if (isPseudoLegalSlidingMove(BBoard, 1UL << rookPosition, 1UL << rookMove, whiteToMove)) {
                printf("%d to %d is a pseudo-legal move\n", rookPosition, rookMove);

                // todo: Check it doesn't put King in danger - generic helper func
                // Make the move, and check if king is attacked by anything.
                // todo: Append to a moves list (might need linked-list structure and helper func)
            }
            rookMoves &= rookMoves - 1;
        }
        rooksBoard &= rooksBoard - 1;
    }
    return NULL;
}

/************************
 * KNIGHT MOVE FUNCTIONS
************************/

/**
 * Generates all (possibly illegal) knight moves
 * @param knight bitboard containing knight position
 * @return bitboard containing the possible positions that knight can move to
 * @cite Multiple Knight Attacks: https://www.chessprogramming.org/Knight_Pattern
 */
uint64_t getKnightMoves(char knight_index) {
    uint64_t knight = 1UL << knight_index;
    uint64_t l1 = (knight >> 1) & not_h_file;
    uint64_t l2 = (knight >> 2) & not_hg_file;
    uint64_t r1 = (knight << 1) & not_a_file;
    uint64_t r2 = (knight << 2) & not_ab_file;
    uint64_t h1 = l1 | r1;
    uint64_t h2 = l2 | r2;
    return (h1<<16) | (h1>>16) | (h2<<8) | (h2>>8);
}


/**
 * Checks if a generated knight move is pseudo-legal or not.
 * @param BBoard Pre-move bitboard
 * @param postMove Position of knight after generated move
 */
bool isPseudoLegalKnightMove(uint64_t *BBoard, uint64_t postMove, bool whiteToMove) {
    return noFriendlyPieceAt(BBoard, postMove, whiteToMove);
}


/**
 * Generates all legal moves that knights can make
 * @param BBoard
 * @param whiteToMove
 * @return An array of move_info pointers that contain all legal knight moves
 */
move *generateMoves_knight(uint64_t *BBoard, bool whiteToMove) {
    // Get knight board of color to move
    uint64_t knightsBoard = BBoard[whiteKnights + !whiteToMove * 7];
#ifdef DEBUG
    printf("Knight board - ");
    render_single(knightsBoard);
#endif
    while (knightsBoard) {
        // For each knight, generate all pseudo-legal moves
        enum enumSquare knightPosition = bitScanForward(knightsBoard);
        uint64_t knightMoves = getKnightMoves(knightPosition);
#ifdef DEBUG
        render_single(knightMoves);
#endif
        while (knightMoves) {
            // For each knight move, check if legal
            enum enumSquare knightMove = bitScanForward(knightMoves);
            if (isPseudoLegalKnightMove(BBoard, 1UL << knightMove, whiteToMove)) {
                printf("%d to %d is a pseudo-legal move\n", knightPosition, knightMove);

                // todo: Check it doesn't put King in danger - generic helper func
                  // Make the move, and check if king is attacked by anything.
                // todo: Append to a moves list (might need linked-list structure and helper func)
            }
            knightMoves &= knightMoves - 1;
        }
        knightsBoard &= knightsBoard - 1;
    }
    return NULL;
}


/**********************
 * MAIN MOVE GENERATION
**********************/

/**
 * Given bitboards and metadata, calculates the best move using MiniMax algorithm
 * @param tokens ie. bitboards, whiteToMove, castling, other FEN info
 * @return move, a pointer to a move_info struct
 */
void *AIMove(FEN tokens) {
    generateMoves_rook(tokens->BBoard, tokens->whiteToMove);
    return 0;
}


/**
 * The meat of script, does anything and everything right now
 * @return An exit code (0 = successful exit)
 */
int main() {
#ifdef DEBUG
    printf("Debugging mode is on!\n");
#endif
    // Input FEN String
    char *board_fen = malloc(sizeof(char) * 100);
    strcpy(board_fen, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");  /// Input a FEN_string here!

    // Extract info from FEN string
    FEN tokens = extract_fen_tokens(board_fen);

#ifdef DEBUG
    render_all(tokens->BBoard);
    int score = evaluateMaterial(tokens->BBoard, tokens->whiteToMove);
    printf("Score: %d \n", score);  // NOTE: need to print negative score if is black playing
#endif

    /// Do AI stuff here;
    AIMove(tokens);

    // Free pointers
    free_tokens(tokens);
    free(board_fen);
    return 0;
}
