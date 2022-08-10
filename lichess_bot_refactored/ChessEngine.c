/**
 * TODO:
 *  0. Learn how to use debugger (set up cmakefile and config)
 *  1. DONE! Create pseudo-legal move generation for all piece types
 *  2. DONE! Complete pseudo-legal checks for sliding pieces (ie. blocking) within move generation
 *  2.5. Do pseudo-legal checks within move generation for non-sliding pieces
 *  2.6. Refactor: each of the getMoves_**** can be condensed into one
 *       Use a list of struct to store what functions each piece uses and loop thru function pointers
 *       Comment code :)
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


/**
 * Checks if a generated knight move is pseudo-legal or not.
 * @param BBoard Pre-move bitboard
 * @param postMove Position of knight after generated move
 */
bool isPseudoLegalNonSlidingMove(uint64_t *BBoard, uint64_t postMove, bool whiteToMove) {
    return noFriendlyPieceAt(BBoard, postMove, whiteToMove);
}


/************************
 * BISHOP MOVE FUNCTIONS
************************/
/**
 * Generates all pseudo-legal bishop moves
 * @param bishop_index index of current bishop
 * @return bitboard containing the possible positions that bishop can move to
 * @cite https://www.chessprogramming.org/Sliding_Piece_Attacks
 */
uint64_t generateBishopMoves(enum enumSquare bishop_index, uint64_t *BBoard, bool whiteToMove) {
    // Get useful boards and initialize attacks board to return
    uint64_t friendlyBoard = BBoard[whiteAll + !whiteToMove * 7];
    uint64_t occupied = BBoard[whiteAll] | BBoard[blackAll];
    uint64_t attacks = 0;
    // Store functions to get four rays of piece in an array to loop through.
    // Require functions to alternate between positive rays (even indices) and negative rays (odd indices)
    uint64_t (*rays[4]) (enum enumSquare sq);
    rays[0] = northEastRay; rays[1] = southWestRay; rays[2] = northWestRay; rays[3] = southEastRay;
    for (int i = 0; i < 4; i++) {
        // Get the square of first piece blocking ray
        uint64_t (*ray)(enum enumSquare sq) = rays[i];
        uint64_t attack = ray(bishop_index);
        uint64_t blockers = attack & occupied;
        enum enumSquare block_sq = 0;
        if (blockers) {
            // Create a ray from this blocker to xor with
            if (i % 2) {block_sq = bitScanForward(blockers);}
            else {block_sq = bitScanReverse(blockers);}
            attack = attack ^ 1UL << block_sq ^ ray(block_sq);
        }
        attacks |= attack;
    }
    // Cannot take any friendly pieces
    return attacks ^ (attacks & friendlyBoard);
}


/**
 * Generates all legal moves that bishops can make
 * @param BBoard
 * @param whiteToMove
 * @return An array of move_info pointers that contain all legal bishop moves
 */
move *getMoves_bishop(uint64_t *BBoard, bool whiteToMove) {
    // Get bishop board of color to move
    uint64_t bishopsBoard = BBoard[whiteBishops + !whiteToMove * 7];
#ifdef DEBUG
    printf("Bishop board - ");
    render_single(bishopsBoard);
#endif
    while (bishopsBoard) {
        // For each bishop, generate all pseudo-legal moves
        enum enumSquare bishopPosition = bitScanForward(bishopsBoard);
        uint64_t bishopMoves = generateBishopMoves(bishopPosition, BBoard, whiteToMove);
#ifdef DEBUG
        printf("Bishop pseudo-legal moves - ");
        render_single(bishopMoves);
#endif
        while (bishopMoves) {
            // For each bishop move, check if legal
            enum enumSquare bishopMove = bitScanForward(bishopMoves);
            printf("%d to %d is a pseudo-legal move\n", bishopPosition, bishopMove);

            // todo: Check it doesn't put King in danger - generic helper func
            // Make the move, and check if king is attacked by anything.
            // todo: Append to a moves list (might need linked-list structure and helper func)
            bishopMoves &= bishopMoves - 1;
        }
        bishopsBoard &= bishopsBoard - 1;
    }
    return NULL;
}


/************************
 * ROOK MOVE FUNCTIONS
************************/
/**
 * Generates all pseudo-legal rook moves
 * @param rook_index index of current rook
 * @return bitboard containing the possible positions that rook can move to
 */
uint64_t generateRookMoves(enum enumSquare rook_index, uint64_t *BBoard, bool whiteToMove) {
    // See generateBishopMoves for code explanation
    uint64_t friendlyBoard = BBoard[whiteAll + !whiteToMove * 7];
    uint64_t occupied = BBoard[whiteAll] | BBoard[blackAll];
    uint64_t attacks = 0;
    uint64_t (*rays[4]) (enum enumSquare sq);
    rays[0] = northRay; rays[1] = southRay; rays[2] = eastRay; rays[3] = westRay;
    for (int i = 0; i < 4; i++) {
        uint64_t (*ray)(enum enumSquare sq) = rays[i];
        uint64_t attack = ray(rook_index);
        uint64_t blockers = attack & occupied;
        enum enumSquare block_sq;
        if (blockers) {
            if (i % 2) {block_sq = bitScanForward(blockers);}
            else {block_sq = bitScanReverse(blockers);}
            attack = attack ^ 1UL << block_sq ^ ray(block_sq);
        }
        attacks |= attack;
    }
    return attacks ^ (attacks & friendlyBoard);
}

/**
 * Generates all legal moves that rooks can make
 * @param BBoard
 * @param whiteToMove
 * @return An array of move_info pointers that contain all legal rook moves
 */
move *getMoves_rook(uint64_t *BBoard, bool whiteToMove) {
    // Get rook board of color to move
    uint64_t rooksBoard = BBoard[whiteRooks + !whiteToMove * 7];
#ifdef DEBUG
    printf("Rook board - ");
    render_single(rooksBoard);
#endif
    while (rooksBoard) {
        // For each rook, generate all pseudo-legal moves
        enum enumSquare rookPosition = bitScanForward(rooksBoard);
        uint64_t rookMoves = generateRookMoves(rookPosition, BBoard, whiteToMove);
#ifdef DEBUG
        printf("Rook pseudo-legal moves - ");
        render_single(rookMoves);
#endif
        while (rookMoves) {
            // For each rook move, check if legal
            enum enumSquare rookMove = bitScanForward(rookMoves);
            printf("%d to %d is a pseudo-legal move\n", rookPosition, rookMove);

            // todo: Check it doesn't put King in danger - generic helper func
            // Make the move, and check if king is attacked by anything.
            // todo: Append to a moves list (might need linked-list structure and helper func)
            rookMoves &= rookMoves - 1;
        }
        rooksBoard &= rooksBoard - 1;
    }
    return NULL;
}


/************************
 * QUEEN MOVE FUNCTIONS
************************/
/**
 * Generates all pseudo-legal queen moves
 * @param queen_index index of current queen
 * @return bitboard containing the possible positions that queen can move to
 */
uint64_t generateQueenMoves(enum enumSquare queen_index, uint64_t *BBoard, bool whiteToMove) {
    return generateRookMoves(queen_index, BBoard, whiteToMove) |
            generateBishopMoves(queen_index, BBoard, whiteToMove);
}


/**
 * Generates all legal moves that queens can make
 * @param BBoard
 * @param whiteToMove
 * @return An array of move_info pointers that contain all legal queen moves
 */
move *getMoves_queen(uint64_t *BBoard, bool whiteToMove) {
    // Get queen board of color to move
    uint64_t queensBoard = BBoard[whiteQueens + !whiteToMove * 7];
#ifdef DEBUG
    printf("Queen board - ");
    render_single(queensBoard);
#endif
    while (queensBoard) {
        // For each queen, generate all pseudo-legal moves
        enum enumSquare queenPosition = bitScanForward(queensBoard);
        uint64_t queenMoves = generateQueenMoves(queenPosition, BBoard, whiteToMove);
#ifdef DEBUG
        printf("Queen pseudo-legal moves - ");
        render_single(queenMoves);
#endif
        while (queenMoves) {
            // For each queen move, check if legal
            enum enumSquare queenMove = bitScanForward(queenMoves);
            printf("%d to %d is a pseudo-legal move\n", queenPosition, queenMove);

            // todo: Check it doesn't put King in danger - generic helper func
            // Make the move, and check if king is attacked by anything.
            // todo: Append to a moves list (might need linked-list structure and helper func)
            queenMoves &= queenMoves - 1;
        }
        queensBoard &= queensBoard - 1;
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
uint64_t generateKnightMoves(enum enumSquare knight_index) {
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
 * Generates all legal moves that knights can make
 * @param BBoard
 * @param whiteToMove
 * @return An array of move_info pointers that contain all legal knight moves
 */
move *getMoves_knight(uint64_t *BBoard, bool whiteToMove) {
    // Get knight board of color to move
    uint64_t knightsBoard = BBoard[whiteKnights + !whiteToMove * 7];
#ifdef DEBUG
    printf("Knight board - ");
    render_single(knightsBoard);
#endif
    while (knightsBoard) {
        // For each knight, generate all pseudo-legal moves
        enum enumSquare knightPosition = bitScanForward(knightsBoard);
        uint64_t knightMoves = generateKnightMoves(knightPosition);
#ifdef DEBUG
        printf("Knight moves - ");
        render_single(knightMoves);
#endif
        while (knightMoves) {
            // For each knight move, check if legal
            enum enumSquare knightMove = bitScanForward(knightMoves);
            if (isPseudoLegalNonSlidingMove(BBoard, 1UL << knightMove, whiteToMove)) {
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


/************************
 * KING MOVE FUNCTIONS
************************/
/**
 * Generates all (possibly illegal) king moves
 * @param king bitboard containing king position
 * @return bitboard containing the possible positions that king can move to
 * @cite Multiple King Attacks: https://www.chessprogramming.org/King_Pattern
 */
uint64_t generateKingMoves(enum enumSquare king_index) {
    // todo: Castling in another function?
    uint64_t king = 1UL << king_index;
    uint64_t l1 = (king >> 1) & not_h_file;
    uint64_t r1 = (king << 1) & not_a_file;
    uint64_t h1 = king | l1 | r1;
    return king ^ (h1 | (h1<<8) | (h1>>8));
}


/**
 * Generates all legal moves that king can make
 * @param BBoard
 * @param whiteToMove
 * @return An array of move_info pointers that contain all legal king moves
 */
move *getMoves_king(uint64_t *BBoard, bool whiteToMove) {
    // Get king board of color to move
    uint64_t kingsBoard = BBoard[whiteKing + !whiteToMove * 7];
#ifdef DEBUG
    printf("King board - ");
    render_single(kingsBoard);
#endif
    // For the king, generate all pseudo-legal moves
    enum enumSquare kingPosition = bitScanForward(kingsBoard);
    uint64_t kingMoves = generateKingMoves(kingPosition);
#ifdef DEBUG
    printf("King moves - ");
    render_single(kingMoves);
#endif
    while (kingMoves) {
        // For each king move, check if legal
        enum enumSquare kingMove = bitScanForward(kingMoves);
        if (isPseudoLegalNonSlidingMove(BBoard, 1UL << kingMove, whiteToMove)) {
            printf("%d to %d is a pseudo-legal move\n", kingPosition, kingMove);

            // todo: Check it doesn't put King in danger - generic helper func
            // Make the move, and check if king is attacked by anything.
            // todo: Append to a moves list (might need linked-list structure and helper func)
        }
        kingMoves &= kingMoves - 1;
    }
    return NULL;
}


/************************
 * PAWN MOVE FUNCTIONS
************************/
uint64_t wSinglePushTargets(uint64_t wpawns, uint64_t empty) {
    return (wpawns << 8) & empty;
}

uint64_t wDoublePushTargets(uint64_t wpawns, uint64_t empty) {
    const uint64_t rank4 = 0x00000000FF000000;
    uint64_t singlePushes = wSinglePushTargets(wpawns, empty);
    return (singlePushes << 8) & empty & rank4;
}

uint64_t wAttackTargets(uint64_t wpawns, uint64_t enemy) {
    return ((wpawns << 7 & not_h_file) | (wpawns << 9 & not_a_file)) & enemy;
}


uint64_t bSinglePushTargets(uint64_t bpawns, uint64_t empty) {
    return (bpawns >> 8) & empty;
}

uint64_t bDoublePushTargets(uint64_t bpawns, uint64_t empty) {
    const uint64_t rank5 = 0x000000FF00000000;
    uint64_t singlePushes = bSinglePushTargets(bpawns, empty);
    return (singlePushes >> 8) & empty & rank5;
}

uint64_t bAttackTargets(uint64_t bpawns, uint64_t enemy) {
    return ((bpawns >> 7 & not_a_file) | (bpawns >> 9 & not_h_file)) & enemy;
}


/**
 * Generates all (possibly illegal) white pawn moves
 * @param pawn bitboard containing pawn position
 * @return bitboard containing the possible positions that pawn can move to
 * @cite https://www.chessprogramming.org/Pawn_Pushes_(Bitboards)
 * @cite https://www.chessprogramming.org/Pawn_Attacks_(Bitboards)
 */
uint64_t generateWhitePawnMoves(uint64_t *BBoard, enum enumSquare pawn_index) {
    uint64_t pawn = 1UL << pawn_index;
    uint64_t empty = ~(BBoard[whiteAll] | BBoard[blackAll]);
    uint64_t pushSet = wSinglePushTargets(pawn, empty) | wDoublePushTargets(pawn, empty);
    uint64_t attackSet = wAttackTargets(pawn, BBoard[blackAll]);
    return pushSet | attackSet;
}


uint64_t generateBlackPawnMoves(uint64_t *BBoard, enum enumSquare pawn_index) {
    uint64_t pawn = 1UL << pawn_index;
    uint64_t empty = !(BBoard[whiteAll] | BBoard[blackAll]);
    uint64_t pushSet = bSinglePushTargets(pawn, empty) | bDoublePushTargets(pawn, empty);
    uint64_t attackSet = bAttackTargets(pawn, BBoard[whiteAll]);
    return pushSet | attackSet;
}


/**
 * Generates all legal moves that pawns can make
 * @param BBoard
 * @param whiteToMove
 * @return An array of move_info pointers that contain all legal pawn moves
 */
move *getMoves_pawn(uint64_t *BBoard, bool whiteToMove) {
    // Get pawn board of color to move
    uint64_t pawnsBoard = BBoard[whitePawns + !whiteToMove * 7];
#ifdef DEBUG
    printf("Pawn board - ");
    render_single(pawnsBoard);
#endif
    while (pawnsBoard) {
        // For each pawn, generate all pseudo-legal moves
        enum enumSquare pawnPosition = bitScanForward(pawnsBoard);
        uint64_t pawnMoves;
        if (whiteToMove) {pawnMoves = generateWhitePawnMoves(BBoard, pawnPosition);}
        else {pawnMoves = generateBlackPawnMoves(BBoard, pawnPosition);}
#ifdef DEBUG
        printf("Pawn moves - ");
        render_single(pawnMoves);
#endif
        while (pawnMoves) {
            // For each pawn move, check if legal
            enum enumSquare pawnMove = bitScanForward(pawnMoves);
            printf("%d to %d is a pseudo-legal move\n", pawnPosition, pawnMove);

            // todo: Check it doesn't put King in danger - generic helper func
            // Make the move, and check if king is attacked by anything.
            // todo: Append to a moves list (might need linked-list structure and helper func)
            pawnMoves &= pawnMoves - 1;
        }
        pawnsBoard &= pawnsBoard - 1;
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
    getMoves_queen(tokens->BBoard, tokens->whiteToMove);
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
    strcpy(board_fen, "rnbqkbnr/pppppppp/8/8/8/8/PP3PPP/RN1Q3R w KQkq - 0 1");  /// Input a FEN_string here!

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
