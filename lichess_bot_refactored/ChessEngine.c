#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "lib/contracts.h"
#include "dataStructs.h"


// Cite: this function is from https://www.chessprogramming.org/BitScan
// Finds index of the least significant 1 bit
int bitScanForward(uint64_t bb) {
    REQUIRES(bb != 0);
    const uint64_t debruijn64 = (0x03f79d71b4cb0a89);
    assert (bb != 0);
    return LS1Bindex64[((bb ^ (bb-1)) * debruijn64) >> 58];
}

/**
 * Flip a bitboard vertically about the centre ranks.
 * Rank 1 is mapped to rank 8 and vice versa.
 * @param x any bitboard
 * @return bitboard x flipped vertically
 * cite: https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#Vertical
 */
uint64_t flipVertical(uint64_t x) {
   const uint64_t k1 = 0x00FF00FF00FF00FF;
   const uint64_t k2 = 0x0000FFFF0000FFFF;
   x = ((x >>  8) & k1) | ((x & k1) <<  8);
   x = ((x >> 16) & k2) | ((x & k2) << 16);
   x = ( x >> 32)       | ( x       << 32);
   return x;
}

void render_single(int64_t currBoard) {
    printf("Board representation: \n");
    currBoard = flipVertical(currBoard);

    for (int r = 0; r < 8; r++) {
        printf("\n");
        for (int c = 0; c < 8; c++) {
            char bitShift = 8 * r + c;
            int64_t position = ((long) 1) << bitShift;
            if (currBoard & position) printf("x ");
            else printf(". ");
        }
    }
    printf("\n\n");
}

// Prints board with a1 at bottom left and h8 at top right
void render_all(uint64_t *BBoard) {
    printf("Board representation: \n");
    char *s = "PNBRQKApnbrqka";  // Used to index into and print pieces

    // Initialize a string board that combines all bitboard pieces
    char boardArray[65];
    for (int i = 0; i < 64; i++) boardArray[i] = '.';
    boardArray[64] = '\0';

    // Combine all bitboards into one string board
    for (enum EPieceType index = whitePawns; index < blackAll; index++) {
        if (index == whiteAll) continue;

        // Board flipped so that a1 at bottom left
        uint64_t currBoard = flipVertical(BBoard[index]);
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 8; c++) {
                int bitShift = 8 * r + c;
                uint64_t position = ((uint64_t) 1) << bitShift;
                if (currBoard & position) {
                    ASSERT(boardArray[bitShift] == '.');  // Must be empty
                    boardArray[bitShift] = s[index];
                }
            }
        }
    }

    // Print the board!
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            printf("%c ", boardArray[8*r + c]);
        }
        printf("\n");
    }
    printf("\n");
}

// Converts board_fen string into bitboard array (see EPieceType for indexes)
// Uses Little-Endian Rank-File Mapping 
// See https://www.chessprogramming.org/Square_Mapping_Considerations#Little-Endian_Rank-File_Mapping
uint64_t *fen2bit(char *board_fen) {
    REQUIRES(board_fen != NULL);
    
    // Create bit board
    uint64_t *bitBoard = calloc(num_pieceTypes, sizeof(uint64_t));    // all 0's
    ASSERT(bitBoard != NULL);  // Equivalent to xcalloc, which isn't working

    
    // Flip the fen board so that a1 is at top left – represented as string array
    char *token = strtok(board_fen, "/");
    char* correct_fen[8];
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
            uint64_t position = ((uint64_t) 1) << bb_index;
            bitBoard[currPieceType] = bitBoard[currPieceType] | position;
            bitBoard[currPieceColor] = bitBoard[currPieceColor] | position;
            bb_index++;
        }
        ASSERT(bb_index == 8*(i+1));
    }

    ENSURES(bitBoard != NULL);
    return bitBoard;
}

// See Brian Kernighan's way (https://www.chessprogramming.org/Population_Count)
// Return the number of pieces (1 bits) on the bitboard
int popCount(uint64_t x) {
    // Alternative: lookup method
      // Brian Kernighan's way: O(3) amortized average | lookup method: O(8)
      //isolates by rank (good for pawn eval?)
    int count = 0;
    while (x) {
        count++;
        x &= x - 1; // reset LS1B
    }
    return count;
}

// This function evaluates material balance of position boards
// Returns sum(weighting * (whiteCount - blackCount)) ie. +ve means white advantage
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

// Test and redundant stuff
char *greeting(char *name) {
    printf("Name received: %s \n", name);
    printf("String length: %lu \n", strlen(name));

    char str1[] = "Hi ";
    char *res = malloc(sizeof(char) * (strlen(str1) + strlen(name)));
    strcpy(res, str1);
    strcat(res, name);
    return res;
}

// mask knight attacks
uint64_t mask_knight_attacks(uint64_t bb_pos)
{
    // attack bitboard
    uint64_t attacks = 0;
    
    // generate knight
    if ((bb_pos >> 17) & not_h_file) attacks |= (bb_pos >> 17);
    if ((bb_pos >> 15) & not_a_file) attacks |= (bb_pos >> 15);
    if ((bb_pos >> 10) & not_hg_file) attacks |= (bb_pos >> 10);
    if ((bb_pos >> 6) & not_ab_file) attacks |= (bb_pos >> 6);
    if ((bb_pos << 17) & not_a_file) attacks |= (bb_pos << 17);
    if ((bb_pos << 15) & not_h_file) attacks |= (bb_pos << 15);
    if ((bb_pos << 10) & not_ab_file) attacks |= (bb_pos << 10);
    if ((bb_pos << 6) & not_hg_file) attacks |= (bb_pos << 6);
    
    // return attack map for knight on a given square
    return attacks;
}
