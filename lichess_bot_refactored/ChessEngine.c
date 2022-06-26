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


// Returns an array of possible moves given bitboard and color to move
move *generateMoves_knight(uint64_t *BBoard, bool whiteToMove) {
    (void) BBoard; (void) whiteToMove;
    printf("Knight board: %d\n", whiteKnights + !whiteToMove * 7);
    uint64_t knightBoard = BBoard[whiteKnights + !whiteToMove * 7];
    int index = bitScanForward(knightBoard);
    (void) index;
    return NULL;
}


/**
 * Given bitboards and metadata, calculates the best move using MiniMax algorithm
 * @param board_fen
 * @return move, a pointer to a move_info struct
 */
void *AIMove(uint64_t *BBoard) {
    return BBoard;
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

    // Free pointers
    free_tokens(tokens);
    free(board_fen);
    return 0;
}
