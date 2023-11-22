//
// Created by Casper Wong on 6/19/22.
//

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "lib/contracts.h"
#include "dataStructs.h"

/**
 * HELPER FUNCTION LOCAL TO THIS FILE. USE flipVertical() IN board_manipulation.h
 * Flip a bitboard vertically about the centre ranks. Rank 1 is mapped to rank 8 and vice versa.
 * @param x any bitboard
 * @return bitboard x flipped vertically
 * cite: https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#Vertical
 */
uint64_t _flipVertical(uint64_t x) {
    const uint64_t k1 = 0x00FF00FF00FF00FF;
    const uint64_t k2 = 0x0000FFFF0000FFFF;
    x = ((x >>  8) & k1) | ((x & k1) <<  8);
    x = ((x >> 16) & k2) | ((x & k2) << 16);
    x = ( x >> 32)       | ( x       << 32);
    return x;
}


void render_single(int64_t bb) {
    printf("Board representation: \n");
    bb = _flipVertical(bb);

    for (int r = 0; r < 8; r++) {
        printf("\n");
        for (int c = 0; c < 8; c++) {
            char bitShift = 8 * r + c;
            int64_t position = ((long) 1) << bitShift;
            if (bb & position) printf("x ");
            else printf(". ");
        }
    }
    printf("\n\n");
}


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
        uint64_t currBoard = _flipVertical(BBoard[index]);
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


uint64_t *fen2bit(char *board_fen) {
    REQUIRES(board_fen != NULL);

    // Create bitboard
    uint64_t *bitBoard = calloc(numPieceTypes, sizeof(uint64_t));    // all 0's
    ASSERT(bitBoard != NULL);

    // Flip the fen board so that a1 is at top left – represented as string array
    char *token = strtok(board_fen, "/");
    char *correct_fen[8];
    ASSERT(correct_fen != NULL);
    for (int i = 7; i >= 0; i--) {
        ASSERT(token != NULL);
        correct_fen[i] = token;
        token = strtok(NULL, "/");
    }

    // Assign each chess piece (char) into it's corresponding bitboard
    for (int i = 0; i < 8; i++) {
        char bb_index = 8 * i;
        char *str = correct_fen[i];

        for (int c = 0; str[c] != '\0'; c++) {
            char curr = (char) str[c];
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


FEN extract_fen_tokens(char *fen_string) {
    FEN tokens = malloc(sizeof(struct FEN_info));

    // Get board_fen
    // Store in temporary string and allocate after extracting tokens (strtok() weirdness)
    fen_string = strtok(fen_string, " ");
    char *board_fen = malloc(sizeof(char)* (strlen(fen_string) + 1));
    ASSERT(board_fen != NULL);
    strcpy(board_fen, fen_string);

    // Get active color
    fen_string = strtok(NULL, " ");
    if (!strcmp(fen_string, "w")) tokens->whiteToMove = 1;
    else tokens->whiteToMove = 0;

    // Get castling rights
    fen_string = strtok(NULL, " ");
    tokens->castling = 0;
    for (int c = 0; fen_string[c] != '\0'; c++) {
        ASSERT(c < 4);
        switch (fen_string[c]) {
            case '-':  // No castling options
                break;
            case 'K':  // King side white
                tokens->castling += 1UL << g1;
                break;
            case 'Q':  // Queen side white
                tokens->castling += 1UL << c1;
                break;
            case 'k':  // King side black
                tokens->castling += 1UL << g8;
                break;
            case 'q':  // Queen side black
                tokens->castling += 1UL << c8;
                break;
        }
    }

    // Get En Passant targets
    fen_string = strtok(NULL, " ");
    tokens->enPassant = 0;
    if (fen_string[0] == '-') {;}  // No en-passant targets. Do nothing!
    else {;}  // Todo: There are en-passant targets (ie. e3). Need to convert this

    // Get halfmoves - draw occurs if 50 halfmoves occur with no piece capture or pawn movement
    fen_string = strtok(NULL, " ");
    tokens->halfMove = atoi(fen_string);  // At most 2 chars

    // Get fullmoves - has no significance really
    fen_string = strtok(NULL, " ");
    tokens->fullMove = atoi(fen_string);  // At most 2 chars

    ASSERT(fen_string != NULL);

    tokens->BBoard = fen2bit(board_fen);
    return tokens;
}


void free_tokens(FEN tokens) {
    free(tokens->BBoard);
    free(tokens);
}


void free_linked_list (node curr) {
    while (curr != NULL) {
        node next = curr->next;
        free(curr->data);
        free(curr);
        curr = next;
    }
}


void enumSquare_to_string(char *res, enum enumSquare square) {
    res[0] = 'a' + square % 8;
    res[1] = '1' + square / 8;
}
