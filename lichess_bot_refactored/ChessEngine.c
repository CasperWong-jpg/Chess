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
 * @param BBoard
 * @return Sum(weighting * (whiteCount - blackCount)) ie. +ve means white advantage
 */
int evaluateMaterial(uint64_t *BBoard, bool whiteToMove) {
    int score = 0;
    int values[] = {100, 300, 300, 500, 900, 20000};  // p, n, b, r, q, k
    for (enum EPieceType i = 0; i < 6; i++) {
        int count = popCount(BBoard[i]) - popCount(BBoard[i + 7]);
        score += values[i] * count;
    }
    if (!whiteToMove)  score *= -1;
    return score;
}


/************************
 * KNIGHT MOVE FUNCTIONS
************************/

/**
 * Generates all *pseudo-legal* knight moves
 * @param knights bitboard containing knight(s) positions
 * @return bitboard containing the possible positions that knight(s) can move to
 * @cite Multiple Knight Attacks: https://www.chessprogramming.org/Knight_Pattern
 */
uint64_t getKnightMoves(uint64_t knights) {
    uint64_t l1 = (knights >> 1) & not_h_file;
    uint64_t l2 = (knights >> 2) & not_hg_file;
    uint64_t r1 = (knights << 1) & not_a_file;
    uint64_t r2 = (knights << 2) & not_ab_file;
    uint64_t h1 = l1 | r1;
    uint64_t h2 = l2 | r2;
    return (h1<<16) | (h1>>16) | (h2<<8) | (h2>>8);
}


/**
 * Checks if a pseudo-legal knight move is legal or not.
 *
 * todo: Think more about params to put in. Could just have the post-move bitboards?
 * @param BBoard Pre-move bitboard
 * @param preMove Position of knight prior to move
 * @param postMove Position of knight after pseudolegal move
 * @return
 */
bool isLegalKnightMove(uint64_t *BBoard, uint64_t preMove, uint64_t postMove) {
    // todo: Knight is not a cannibal - check it isn't eating a piece of same color - generic helper func

    // todo: Check it doesn't put King in danger - generic helper func
      // Make the move, and check if king is attacked by anything
    return true;
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
        uint64_t knightMoves = getKnightMoves(1UL << knightPosition);
#ifdef DEBUG
        render_single(knightMoves);
#endif
        while (knightMoves) {
            // For each knight move, check if legal
            enum enumSquare knightMove = bitScanForward(knightMoves);
            if (isLegalKnightMove(BBoard, 1UL << knightPosition, 1UL << knightMove)) {
                // todo: Append to a moves list (might need linked-list structure and helper func)
                printf("%d to %d is a valid move\n", knightPosition, knightMove);
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
    generateMoves_knight(tokens->BBoard, tokens->whiteToMove);
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
