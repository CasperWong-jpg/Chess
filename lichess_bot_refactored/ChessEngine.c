#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lib/contracts.h"
#include "dataStructs.h"
#include "dev_tools.h"
#include "board_manipulations.h"

/**
 * Evaluates material balance of position boards
 * @param BBoard
 * @return Sum(weighting * (whiteCount - blackCount)) ie. +ve means white advantage
 */
int evaluateMaterial(uint64_t *BBoard) {
    int score = 0;
    int values[] = {100, 300, 300, 500, 900, 20000};  // p, n, b, r, q, k
    for (enum EPieceType i = 0; i < 6; i++) {
        int count = popCount(BBoard[i]) - popCount(BBoard[i + 7]);
        score += values[i] * count;
    }
    return score;
}

/*
Move generation algorithm:
  - Generate all moves for a piece 
    - Categorize by piece type: knight, king, pawn, sliding pieces 
    - Represent move using a struct
    - Return an array of these moves
*/

// Returns an array of possible moves given bitboard and color to move
move *generateMoves_knight(uint64_t *BBoard, bool blackToMove) {
    (void) BBoard; (void) blackToMove;
    printf("Knight board: %d\n", whiteKnights + blackToMove * 7);
    uint64_t knightBoard = BBoard[whiteKnights + blackToMove * 7];
    int index = bitScanForward(knightBoard);
    (void) index;
    return NULL;
}


// This function suggests best move given a current board
// Example input: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
// Example output: "e2e4"
char *AIMove(char *board_fen) {
#ifdef DEBUG
    printf("Debugging mode is on!\n");
#endif

    // Change FEN string to bitboard
    strtok(board_fen, " ");
    char* token = malloc(sizeof(char)* (strlen(board_fen) + 1));
    strcpy(token, board_fen);

    // Get other important parameters: color, castling, en passant
    // See: https://www.chess.com/terms/fen-chess for turns
    bool blackToMove = (strtok(NULL, " ")[0] == 'w');  // 1 = black; 0 = white
    printf("Black to move: %d\n", blackToMove);

    while (board_fen != NULL) {
        board_fen = strtok(NULL, " ");
        printf("Remaining token: %s\n", board_fen);  // printing each token
        // TODO: extract these tokens!
    }
    // Generate knight moves
    
    uint64_t *BBoard_pos = fen2bit(token);
    render_all(BBoard_pos);

    free(token);
#ifdef DEBUG
    render_all(BBoard_pos);
    // Evaluate board
    int score = evaluateMaterial(BBoard_pos);
    // NOTE: need to print negative score if is black playing, get in next token
    printf("Score: %d \n", score);
#endif

    free(BBoard_pos);
    return NULL;
}

int main() {
    char s[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    AIMove(s);
    return 0;
}

/*
Process:    
1. Convert board_fen into bitboard (board visualization) DONE
2. Board position (ie. value of each piece)
  5. Positioning heuristics (adding on to board position)
3. Move generation
4. Minimax (then alpha beta pruning)

Helper functions:
- fen2bit(str) MADE
– generateMoves(bitboards, color, castling, pawnPromotion)
– evalBoard(bitboards)
– minimize & maximize etc.

– renderall(bitboards) for debugging purposes MADE
*/
