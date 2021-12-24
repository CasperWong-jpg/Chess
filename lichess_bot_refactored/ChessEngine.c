#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lib/contracts.h"

#define num_pieceTypes 14

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


void render(int64_t *bbBoard, int index) {
    char *s = "PNBRQKApnbrqka";
    printf("Board representation for %c: \n", s[index]);
    if (index == 14) {
        for (int i = 0; i < num_pieceTypes; i++){
            return;
        }
    }
    else {
        ASSERT(0 <= index && index < 14);
        int64_t currBoard = bbBoard[index];
        for (int r = 0; r < 8; r++) {
            printf("\n");
            for (int c = 0; c < 8; c++) {
                char bitShift = 8 * r + c;
                int64_t position = ((long) 1) << bitShift;
                if (currBoard & position) printf("x ");
                else printf(". ");
            }
        }
    }
    printf("\n\n");
}

void renderAll(int64_t *bbBoard) {
    for (int i = 0; i < 14; i++) {
        render(bbBoard, i);
    }
}

// Converts board_fen string into bitboard array (see EPieceType for indexes)
// Uses Little-Endian Rank-File Mapping 
// See https://www.chessprogramming.org/Square_Mapping_Considerations#Little-Endian_Rank-File_Mapping
int64_t *fen2bit(char *board_fen) {
    REQUIRES(board_fen != NULL);
    
    // Create bit board
    int64_t *bitBoard = calloc(num_pieceTypes, sizeof(int64_t));    // all 0's
    ASSERT(bitBoard != NULL);  // Equivalent to xcalloc, which isn't working

    
    // Flip the fen board so that a1 is at top left – represented as string array
    char *token = strtok(board_fen, "/");
    char **correct_fen = malloc(8 * sizeof(char*));
    ASSERT(correct_fen != NULL);
    for (int i = 7; i >= 0; i--) {
        ASSERT(token != NULL);
        correct_fen[i] = token;
        token = strtok(NULL, "/");
    }

    // Assign each chess piece (char) into it's corresponding bit board
    for (int i = 0; i < 8; i++) {
        char bb_index = 8 * i;
        char *str = correct_fen[i];

        for (int c = 0; str[c] != '\0'; c++) {
            char curr = (int) str[c];
            if ('0' < curr && curr <= '8') {    // char is number – empty square
                bb_index = bb_index + (curr - '0');
                continue;
            }

            enum EPieceType currPieceType;
            enum EPieceType currPieceColor;
            switch (curr) {
                case 'P':
                    currPieceType = whitePawns;
                    currPieceColor = whiteAll;
                    break;
                case 'N':
                    currPieceType = whiteKnights;
                    currPieceColor = whiteAll;
                    break;
                case 'B':
                    currPieceType = whiteBishops;
                    currPieceColor = whiteAll;
                    break;
                case 'R':
                    currPieceType = whiteRooks;
                    currPieceColor = whiteAll;
                    break;
                case 'Q':
                    currPieceType = whiteQueens;
                    currPieceColor = whiteAll;
                    break;
                case 'K':
                    currPieceType = whiteKing;
                    currPieceColor = whiteAll;
                    break;
                case 'p':
                    currPieceType = blackPawns;
                    currPieceColor = blackAll;
                    break;
                case 'n':
                    currPieceType = blackKnights;
                    currPieceColor = blackAll;
                    break;
                case 'b':
                    currPieceType = blackBishops;
                    currPieceColor = blackAll;
                    break;
                case 'r':
                    currPieceType = blackRooks;
                    currPieceColor = blackAll;
                    break;
                case 'q':
                    currPieceType = blackQueens;
                    currPieceColor = blackAll;
                    break;
                case 'k':
                    currPieceType = blackKing;
                    currPieceColor = blackAll;
                    break;
                default:
                    assert(false);  // shouldn't be any other string
            }
            ASSERT(0 <= bb_index && bb_index < 64);
            // printf("currPiece: %c, index: %d\n", curr, bb_index);
            int64_t position = ((long) 1) << bb_index;
            bitBoard[currPieceType] = bitBoard[currPieceType] | position;
            bitBoard[currPieceColor] = bitBoard[currPieceColor] | position;
            bb_index++;
        }
        ASSERT(bb_index == 8*(i+1));
    }

    free(correct_fen);
    ENSURES(bitBoard != NULL);
    return bitBoard;
}

// This function suggests best move given a current board
// Example input: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
// Example output: "e2e4"
char *AIMove(char *board_fen) {
    #ifdef DEBUG
    printf("Debugging mode is on!\n");
    #endif

    char *token = strtok(board_fen, " ");
    printf("We made it here\n");
    int64_t *bbBoard_pos = fen2bit(token);

    renderAll(bbBoard_pos);


    token = strtok(NULL, " ");
    printf("Remaining token: %s\n", token);  //printing each token

    free(bbBoard_pos);
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
2. Move generation
3. Board position (ie. value of each piece)
4. Minimax (then alpha beta pruning)

Helper functions:
- fen2bit(str) MADE
– generateMoves(bitboards, color, castling, pawnPromotion)
– evalBoard(bitboards)
– minimize & maximize etc.

– renderall(bitboards) for debugging purposes MADE
*/

// Test stuff
char *greeting(char *name) {
    printf("Name received: %s \n", name);
    printf("String length: %lu \n", strlen(name));

    char str1[] = "Hi ";
    char *res = malloc(sizeof(char) * (strlen(str1) + strlen(name)));
    strcpy(res, str1);
    strcat(res, name);
    return res;
}
