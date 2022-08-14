/**
 * TODO:
 *  0. Learn how to use debugger (set up cmakefile and config)
 *  1. DONE! Create pseudo-legal move generation for all piece types
 *  2. DONE! Complete pseudo-legal checks for sliding pieces (ie. blocking) within move generation
 *  2.5. DONE! Do pseudo-legal checks within move generation for non-sliding pieces
 *  3. Refactor: each of the getMoves_**** can be condensed into one
 *       DONE: Use a list of struct to store what functions each piece uses and loop thru function pointers
 *       DONE: King and Pawn take additional arguments
 *       Comment code :)
 *  4. Castling (take tokens as arguments) + en passant?
 *  5. Do legality checks (tryMove & isInCheck)
 *  6. MiniMax!!
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/contracts.h"
#include "dataStructs.h"
#include "dev_tools.h"
#include "board_manipulations.h"

#ifndef DEBUG
#define DEBUG  // Use #undef to disable debugging behaviour
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
 * Check whether king of specified color is put in check. Used for legality checking.
 * @param BBoard
 * @param checkWhite
 * @return
 */
bool isInCheck(uint64_t *BBoard, bool checkWhite) {
    return false;
}


/************************
 * SLIDING MOVE FUNCTIONS
************************/
uint64_t generateSlidingMoves(enum enumSquare index, uint64_t *BBoard, bool whiteToMove,
                              uint64_t (*rays[4]) (enum enumSquare)) {
    // Get useful boards and initialize attacks board to return
    uint64_t friendlyBoard = BBoard[whiteAll + !whiteToMove * 7];
    uint64_t occupied = BBoard[whiteAll] | BBoard[blackAll];
    uint64_t attacks = 0;
    for (int i = 0; i < 4; i++) {
        // Loop through the attack rays
        uint64_t (*ray)(enum enumSquare sq) = rays[i];
        // Get attack squares and blocking pieces
        uint64_t attack = ray(index);
        uint64_t blockers = attack & occupied;
        if (blockers) {
            // Get the square of first piece blocking ray and create an attack ray from here
            enum enumSquare block_sq;
            if (i % 2) {block_sq = bitScanReverse(blockers);}  // Odd indexes store negative rays
            else {block_sq = bitScanForward(blockers);}  // Even indexes store positive rays
            // Subtract blocker rays from attack ray
            attack = attack ^ ray(block_sq);
        }
        attacks |= attack;
    }
    return attacks ^ (attacks & friendlyBoard);
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
    // Store functions to get four rays of piece in an array to loop through.
    // Require functions to alternate between positive rays (even indices) and negative rays (odd indices)
    uint64_t (*rays[4]) (enum enumSquare sq);
    rays[0] = northEastRay; rays[1] = southWestRay; rays[2] = northWestRay; rays[3] = southEastRay;
    return generateSlidingMoves(bishop_index, BBoard, whiteToMove, rays);
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
    // Store functions to get four rays of piece in an array to loop through.
    // Require functions to alternate between positive rays (even indices) and negative rays (odd indices)
    uint64_t (*rays[4]) (enum enumSquare sq);
    rays[0] = northRay; rays[1] = southRay; rays[2] = eastRay; rays[3] = westRay;

    return generateSlidingMoves(rook_index, BBoard, whiteToMove, rays);
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


/************************
 * KNIGHT MOVE FUNCTIONS
************************/
/**
 * Generates all pseudo-legal knight moves
 * @param knight bitboard containing knight position
 * @return bitboard containing the possible positions that knight can move to
 * @cite Multiple Knight Attacks: https://www.chessprogramming.org/Knight_Pattern
 */
uint64_t generateKnightMoves(enum enumSquare knight_index, uint64_t *BBoard, bool whiteToMove) {
    uint64_t friendlyBoard = BBoard[whiteAll + !whiteToMove * 7];
    uint64_t knight = 1UL << knight_index;
    uint64_t l1 = (knight >> 1) & not_h_file;
    uint64_t l2 = (knight >> 2) & not_hg_file;
    uint64_t r1 = (knight << 1) & not_a_file;
    uint64_t r2 = (knight << 2) & not_ab_file;
    uint64_t h1 = l1 | r1;
    uint64_t h2 = l2 | r2;
    uint64_t moveSet = (h1<<16) | (h1>>16) | (h2<<8) | (h2>>8);
    return moveSet ^ (moveSet & friendlyBoard);
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
uint64_t generateKingMoves(enum enumSquare king_index, uint64_t *BBoard, bool whiteToMove, uint64_t castling) {
    // todo: Castling in another function?
    uint64_t friendlyBoard = BBoard[whiteAll + !whiteToMove * 7];
    uint64_t king = 1UL << king_index;
    uint64_t l1 = (king >> 1) & not_h_file;
    uint64_t r1 = (king << 1) & not_a_file;
    uint64_t h1 = king | l1 | r1;
    uint64_t moveSet = king ^ (h1 | (h1<<8) | (h1>>8));
    return moveSet ^ (moveSet & friendlyBoard);
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
 * Generates all pseudo-legal white pawn moves
 * @param pawn bitboard containing pawn position
 * @return bitboard containing the possible positions that pawn can move to
 * @cite https://www.chessprogramming.org/Pawn_Pushes_(Bitboards)
 * @cite https://www.chessprogramming.org/Pawn_Attacks_(Bitboards)
 */
uint64_t generateWhitePawnMoves(enum enumSquare pawn_index, uint64_t *BBoard) {
    uint64_t pawn = 1UL << pawn_index;
    uint64_t empty = ~(BBoard[whiteAll] | BBoard[blackAll]);
    uint64_t pushSet = wSinglePushTargets(pawn, empty) | wDoublePushTargets(pawn, empty);
    uint64_t attackSet = wAttackTargets(pawn, BBoard[blackAll]);
    return pushSet | attackSet;
}

/**
 * Generates all pseudo-legal black pawn moves
 */
uint64_t generateBlackPawnMoves(enum enumSquare pawn_index, uint64_t *BBoard) {
    uint64_t pawn = 1UL << pawn_index;
    uint64_t empty = ~(BBoard[whiteAll] | BBoard[blackAll]);
    uint64_t pushSet = bSinglePushTargets(pawn, empty) | bDoublePushTargets(pawn, empty);
    uint64_t attackSet = bAttackTargets(pawn, BBoard[whiteAll]);
    return pushSet | attackSet;
}

/**
 * Wrapper function that calls the pawn move generation function for corresponding color
 */
uint64_t generatePawnMoves(enum enumSquare pawn_index, uint64_t *BBoard, bool whiteToMove, uint64_t enPassant) {
    if (whiteToMove) {return generateWhitePawnMoves(pawn_index, BBoard);}
    else {return generateBlackPawnMoves(pawn_index, BBoard);}
}


/**********************
 * MAIN MOVE GENERATION
**********************/
/**
 * Initialize struct that iterates through all piece types and the corresponding functions they use in move generation
 * @return A pointer to generic_get_move_struct struct
 */
generic_get_move get_pieces_struct(void *castling, void *enPassant) {
    generic_get_move head = malloc(sizeof(struct generic_get_move_struct));

    // Initialize for pawns
    generic_get_move piece_list = head;
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whitePawns;
    piece_list->move_gen_func_ptr.additional = &generatePawnMoves;
    piece_list->initialized = true;
    piece_list->additional_data = enPassant;
    piece_list->next = malloc(sizeof(struct generic_get_move_struct));

    // Initialize for knights
    piece_list = piece_list->next;
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whiteKnights;
    piece_list->move_gen_func_ptr.normal = &generateKnightMoves;
    piece_list->initialized = false;
    piece_list->next = malloc(sizeof(struct generic_get_move_struct));

    // Initialize for bishops
    piece_list = piece_list->next;
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whiteBishops;
    piece_list->move_gen_func_ptr.normal = &generateBishopMoves;
    piece_list->initialized = false;
    piece_list->next = malloc(sizeof(struct generic_get_move_struct));

    // Initialize for rooks
    piece_list = piece_list->next;
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whiteRooks;
    piece_list->move_gen_func_ptr.normal = &generateRookMoves;
    piece_list->initialized = false;
    piece_list->next = malloc(sizeof(struct generic_get_move_struct));

    // Initialize for queens
    piece_list = piece_list->next;
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whiteQueens;
    piece_list->move_gen_func_ptr.normal = &generateQueenMoves;
    piece_list->initialized = false;
    piece_list->next = malloc(sizeof(struct generic_get_move_struct));

    // Initialize for king
    piece_list = piece_list->next;
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whiteKing;
    piece_list->move_gen_func_ptr.additional = &generateKingMoves;
    piece_list->initialized = true;
    piece_list->additional_data = castling;
    piece_list->next = NULL;

    return head;
}

/**
 * Frees a linked list. This can be moved to dev_tools and made generic (using void *).
 * @param head Head of a linked list
 */
void free_pieces_struct (generic_get_move head) {
    generic_get_move curr = head;
    while (curr != NULL) {
        generic_get_move next = curr->next;
        free(curr);
        curr = next;
    }
}


/**
 * Generates all legal moves for player to move
 * @param BBoard
 * @param whiteToMove Player to move
 * @param castling Bitboard containing all castling squares (all colors included)
 * @param enPassant Bitboard containing all en-passant squares (all colors included)
 * @return An linked list of move_info pointers that contain all legal knight moves
 */
move *getMoves(uint64_t *BBoard, bool whiteToMove, void *castling, void *enPassant) {
    generic_get_move piece_list = get_pieces_struct(castling, enPassant);

    for (generic_get_move piece = piece_list; piece != NULL; piece = piece->next) {
        uint64_t pieceBoard = BBoard[piece->pieceType + !whiteToMove * 7];
#ifdef DEBUG
        printf("Piece board - ");
        render_single(pieceBoard);
#endif
        while (pieceBoard) {
            // For each knight, generate all pseudo-legal moves
            enum enumSquare piece_index = bitScanForward(pieceBoard);
            uint64_t pieceMoves;
            if (piece->initialized) {
                pieceMoves = (piece->move_gen_func_ptr.additional)(
                        piece_index, BBoard, whiteToMove, piece->additional_data
                );
            }
            else {pieceMoves = (piece->move_gen_func_ptr.normal)(piece_index, BBoard, whiteToMove);}
#ifdef DEBUG
            printf("Piece moves - ");
            render_single(pieceMoves);
#endif
            while (pieceMoves) {
                // For each move, check if legal
                enum enumSquare pieceMove = bitScanForward(pieceMoves);

                // todo: Check it doesn't put King in danger - generic helper func
                // Make the move, and check if king is attacked by anything.
                // todo: Append to a moves list (might need linked-list structure and helper func)
                pieceMoves &= pieceMoves - 1;
            }
            pieceBoard &= pieceBoard - 1;
        }
        // Append moves here
    }
    free_pieces_struct(piece_list)
    return NULL;
}


/**
 * Given bitboards and metadata, calculates the best move using MiniMax algorithm
 * @param tokens ie. bitboards, whiteToMove, castling, other FEN info
 * @return move, a pointer to a move_info struct
 */
void *AIMove(FEN tokens) {
    getMoves(tokens->BBoard, tokens->whiteToMove, tokens->castling, tokens->enPassant);
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
    strcpy(board_fen, "rnbqkbnr/pppppppp/8/8/8/8/PP3PPP/RNBQK2R w KQkq - 0 1");  /// Input a FEN_string here!

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
