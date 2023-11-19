/**
 *  0. Learn how to use debugger (set up cmakefile and config)
 *  1. DONE! Create pseudo-legal move generation for all piece types
 *  2. DONE! Complete pseudo-legal checks for sliding pieces (ie. blocking) within move generation
 *  2.5. DONE! Do pseudo-legal checks within move generation for non-sliding pieces
 *  3. DONE! Refactor: each of the getMoves_**** can be condensed into one
 *       Use a list of struct to store what functions each piece uses and loop thru function pointers
 *       King and Pawn take additional arguments
 *       Comment code :)
 *  4. TODO: (Optional) Castling (take tokens as arguments) + en passant
 *  5. DONE! Do legality checks (tryMove & isInCheck)
 *  6. Done! MiniMax!!
 *  7. TODO: Alpha-beta pruning!
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "lib/contracts.h"
#include "dataStructs.h"
#include "dev_tools.h"
#include "board_manipulations.h"

#ifndef DEBUG
#define DEBUG (0)  // Debug is off by default
#endif

#define DEPTH (4)  // Number of moves to think ahead

/********************
 * GENERIC AI HELPERS
*********************/
/**
 * Evaluates material balance of position boards
 * @return Sum(weighting * (playingColorCount - waitingColorCount)) ie. +ve means advantage for playing team
 */
int evaluateMaterial(uint64_t *BBoard, bool whiteToMove) {
    int score = 0;
    int values[] = {100, 300, 300, 500, 900, 20000};  // p, n, b, r, q, k
    for (enum EPieceType i = 0; i < whiteAll; i++) {
        int count = popCount(BBoard[i]) - popCount(BBoard[i + colorOffset]);
        score += values[i] * count;
    }
    if (!whiteToMove) score *= -1;
    return score;
}


/************************
 * SLIDING MOVE FUNCTIONS
************************/
uint64_t generateSlidingMoves(enum enumSquare index, uint64_t *BBoard, bool whiteToMove,
                              uint64_t (*rays[4]) (enum enumSquare)) {
    // Get useful boards and initialize attacks board to return
    uint64_t friendlyBoard = BBoard[whiteAll + !whiteToMove * colorOffset];
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
    uint64_t friendlyBoard = BBoard[whiteAll + !whiteToMove * colorOffset];
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
    (void) castling;
    // todo: Castling in another function?
    uint64_t friendlyBoard = BBoard[whiteAll + !whiteToMove * colorOffset];
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
    (void) enPassant;
    if (whiteToMove) {return generateWhitePawnMoves(pawn_index, BBoard);}
    else {return generateBlackPawnMoves(pawn_index, BBoard);}
}


/**********************
 * MAIN MOVE HELPERS
**********************/
/**
 * Initialize a linked list, where data contains: piece types + corresponding functions used in move generation
 * @return Head of a generic linked list, which contains generic_get_move pointers as data
 */
node get_pieces_struct(uint64_t castling, uint64_t enPassant) {
    /// Wrap below into a for loop, and change it so that pieceType can be black or white (then we can delete offsets)
    node head = malloc(sizeof(node));
    node piece_node = head;

    // Initialize for pawns
    generic_get_move piece_list = malloc(sizeof(struct generic_get_move_struct));
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whitePawns;
    piece_list->move_gen_func_ptr.additional = &generatePawnMoves;
    piece_list->initialized = true;
    piece_list->additional_data = enPassant;
    piece_node->data = (void *) piece_list;
    piece_node->next = malloc(sizeof(node));

    // Initialize for knights
    piece_node = piece_node->next;
    piece_list = malloc(sizeof(struct generic_get_move_struct));
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whiteKnights;
    piece_list->move_gen_func_ptr.normal = &generateKnightMoves;
    piece_list->initialized = false;
    piece_node->data = (void *) piece_list;
    piece_node->next = malloc(sizeof(node));

    // Initialize for bishops
    piece_node = piece_node->next;
    piece_list = malloc(sizeof(struct generic_get_move_struct));
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whiteBishops;
    piece_list->move_gen_func_ptr.normal = &generateBishopMoves;
    piece_list->initialized = false;
    piece_node->data = (void *) piece_list;
    piece_node->next = malloc(sizeof(node));

    // Initialize for rooks
    piece_node = piece_node->next;
    piece_list = malloc(sizeof(struct generic_get_move_struct));
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whiteRooks;
    piece_list->move_gen_func_ptr.normal = &generateRookMoves;
    piece_list->initialized = false;
    piece_node->data = (void *) piece_list;
    piece_node->next = malloc(sizeof(node));

    // Initialize for queens
    piece_node = piece_node->next;
    piece_list = malloc(sizeof(struct generic_get_move_struct));
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whiteQueens;
    piece_list->move_gen_func_ptr.normal = &generateQueenMoves;
    piece_list->initialized = false;
    piece_node->data = (void *) piece_list;
    piece_node->next = malloc(sizeof(node));

    // Initialize for king
    piece_node = piece_node->next;
    piece_list = malloc(sizeof(struct generic_get_move_struct));
    ASSERT(piece_list != NULL);
    piece_list->pieceType = whiteKing;
    piece_list->move_gen_func_ptr.additional = &generateKingMoves;
    piece_list->initialized = true;
    piece_list->additional_data = castling;
    piece_node->data = (void *) piece_list;
    piece_node->next = NULL;

    return head;
}


/*******************
 * LEGALITY CHECKING
*******************/
/**
 * Check whether king of specified color is put in check. Used for legality checking.
 * @param BBoard
 * @param checkWhite
 * @return
 */
bool isInCheck(uint64_t *BBoard, bool whiteMoved) {
    /**
     Pseudocode:
     - Get king board for friendly color, and all piece type boards for enemy color
     - Loop through each enemy color board, and generate moves (could use get_pieces_struct)
       - Intersect with king board. Return true if bitboard not empty
     - After all loops, return false!
    */
    // Get king board for friendly color, and all piece types
    uint64_t kingBoard = BBoard[whiteKing + !whiteMoved * colorOffset];
    node piece_list = get_pieces_struct(0, 0);  // Castling and en-passant irrelevant

    for (node piece_node = piece_list; piece_node != NULL; piece_node = piece_node->next) {
        // Loop through enemy piece types & boards
        generic_get_move piece = (generic_get_move) piece_node->data;
        uint64_t pieceBoard = BBoard[piece->pieceType + whiteMoved * colorOffset];

        while (pieceBoard) {
            // For each piece, generate all pseudo-legal moves
            enum enumSquare piece_index = bitScanForward(pieceBoard);
            uint64_t pieceMoves;
            if (piece->initialized) {
                pieceMoves = (piece->move_gen_func_ptr.additional)(
                        piece_index, BBoard, !whiteMoved, piece->additional_data
                );
            } else {
                pieceMoves = (piece->move_gen_func_ptr.normal)(piece_index, BBoard, !whiteMoved);
            }
            if (pieceMoves & kingBoard) {  // Friendly king within enemy moves list, in check!
                return true;
            }
            pieceBoard &= pieceBoard - 1;
        }
    }
    return false;  // King is not in moves list of any enemy piece, so is safe :)
}


/**
 * Make move to see if it is legal
 * @param BBoard
 * @param whiteToMove
 * @param m
 * @return
 */
bool checkMoveLegal(uint64_t *BBoard, bool whiteToMove, move m) {
    // Make move on a copy of BBoard, since we want to unmake the move
    uint64_t *tmpBBoard = malloc(numPieceTypes * sizeof(uint64_t));
    memcpy(tmpBBoard, BBoard, numPieceTypes * sizeof(uint64_t));

    for (enum EPieceType i = 0; i < numPieceTypes; i++) {
        ASSERT(BBoard[i] == tmpBBoard[i]);
    }

    make_move(tmpBBoard, m);
    bool inCheck = isInCheck(tmpBBoard, whiteToMove);

    // Free the temporary bitboard
    free(tmpBBoard);
    return !inCheck;
}


/**********************
 * MAIN MOVE GENERATION
**********************/
/**
 * Generates all legal moves for player to move
 * @param BBoard
 * @param whiteToMove Player to move
 * @param castling Bitboard containing all castling squares (all colors included)
 * @param enPassant Bitboard containing all en-passant squares (all colors included)
 * @return An linked list of move_info pointers that contain all legal knight moves
 */
node getMoves(uint64_t *BBoard, bool whiteToMove, uint64_t castling, uint64_t enPassant) {
    node piece_list = get_pieces_struct(castling, enPassant);
    node move_head = NULL;
    node move_list = move_head;

    for (node piece_node = piece_list; piece_node != NULL; piece_node = piece_node->next) {
        generic_get_move piece = (generic_get_move) piece_node->data;
        uint64_t pieceBoard = BBoard[piece->pieceType + !whiteToMove * colorOffset];
#if DEBUG
        printf("Piece board for piece type %d - ", piece->pieceType);
        render_single(pieceBoard);
#endif
        while (pieceBoard) {
            // For each piece, generate all pseudo-legal moves
            enum enumSquare piece_index = bitScanForward(pieceBoard);
            uint64_t pieceMoves;
            if (piece->initialized) {  // Kings & pawns have additional status (castling & en passant) to keep track of
                pieceMoves = (piece->move_gen_func_ptr.additional)(
                        piece_index, BBoard, whiteToMove, piece->additional_data
                );
            }
            else {  // Normal pieces have normal function signature without additional uint64_t argument
                pieceMoves = (piece->move_gen_func_ptr.normal)(piece_index, BBoard, whiteToMove);
            }
#if DEBUG
            printf("Piece moves - ");
            render_single(pieceMoves);
#endif
            while (pieceMoves) {
                // For each move, check if legal
                enum enumSquare piece_move = bitScanForward(pieceMoves);
                move m = malloc(sizeof(struct move_info));
                m->from = piece_index;
                m->to = piece_move;
                m->piece = piece->pieceType + !whiteToMove * colorOffset;
                if (checkMoveLegal(BBoard, whiteToMove, m)) {
                    // Legal move: add to list of possible moves
                    if (move_list == NULL) {
                        move_list = malloc(sizeof(struct Node));
                        move_head = move_list;
                    }
                    else {
                        move_list->next = malloc(sizeof(struct Node));
                        move_list = move_list->next;
                    }
                    move_list->data = (void *) m;
                    move_list->next = NULL;
                }
                else {
                    // Illegal move: disregard it
#if DEBUG
                    printf("%d to %d is not a legal move\n", m->from, m->to);
#endif
                    free(m);
                }
                pieceMoves &= pieceMoves - 1;
            }
            pieceBoard &= pieceBoard - 1;
        }
    }
    free_linked_list(piece_list);
    return move_head;
}


/**
 *
 * @param depth
 * @return
 * @cite https://www.chessprogramming.org/Negamax
 */
int negaMax(uint64_t *BBoard, bool whiteToMove, uint64_t castling, uint64_t enPassant, uint32_t depth) {
    if (depth == 0) {
        int score = evaluateMaterial(BBoard, whiteToMove);
        return score;
    }

    node move_list = getMoves(BBoard, whiteToMove, castling, enPassant);
    int bestScore = INT_MIN;
    uint64_t *tmpBBoard = malloc(numPieceTypes * sizeof(uint64_t));
    for (node move_node = move_list; move_node != NULL; move_node = move_node->next) {
        // Make move, evaluate it, (and unmake move if necessary)
        move m = (move) move_node->data;
        memcpy(tmpBBoard, BBoard, numPieceTypes * sizeof(uint64_t));
        make_move(tmpBBoard, m);
        int currScore = -1 * negaMax(tmpBBoard, !whiteToMove, castling, enPassant, depth-1);

        // If this is the best move, then return this score
        if (currScore > bestScore) {
            bestScore = currScore;
        }
    }

    free(tmpBBoard);
    free_linked_list(move_list);
    return bestScore;
}


/**
 * Given bitboards and metadata, calculates the best move using MiniMax algorithm
 * @param tokens ie. bitboards, whiteToMove, castling, other FEN info
 * @param bestMove Pointer to the best move
 * @return move, a pointer to a move_info struct
 */
move AIMove(FEN tokens, move bestMove) {
    // Generate all possible legal moves
    uint64_t *BBoard = tokens->BBoard;
    bool whiteToMove = tokens->whiteToMove;
    uint64_t castling = tokens->castling;
    uint64_t enPassant = tokens->enPassant;
    node move_list = getMoves(BBoard, whiteToMove, castling, enPassant);
    if (move_list == NULL) {
        printf("No moves possible. Stalemate or checkmate\n");
        exit(1);
    }

    // With all possible moves, use negaMax to find best move
    // AIMove serves as a root negaMax, which returns the best move instead of score
    int depth = DEPTH;
    ASSERT(depth > 0);
    int bestScore = INT_MIN;
    uint64_t *tmpBBoard = malloc(numPieceTypes * sizeof(uint64_t));
    for (node move_node = move_list; move_node != NULL; move_node = move_node->next) {
        // Make move, evaluate it, (and unmake move if necessary)
        move m = (move) move_node->data;
        if (m->from == e4 && m->to == d5) {
            printf("This should be money move\n");
        }
        memcpy(tmpBBoard, BBoard, numPieceTypes * sizeof(uint64_t));
        make_move(tmpBBoard, m);
        int currScore = -negaMax(tmpBBoard, !whiteToMove, castling, enPassant, depth-1);

        // If this is the best move, then store it
        if (currScore > bestScore) {
            bestScore = currScore;
            bestMove->from = m->from;
            bestMove->to = m->to;
            bestMove->piece = m->piece;
        }
    }

    free(tmpBBoard);
    free_linked_list(move_list);
    return bestMove;
}


/**
 * The meat of script, does anything and everything right now
 * @return An exit code (0 = successful exit)
 */
int main(void) {
#if DEBUG
    printf("Debugging mode is on!\n");
#endif
    // Input FEN String
    char *board_fen = malloc(sizeof(char) * 100);
    strcpy(board_fen, "rnbqkbnr/ppN2ppp/4p3/3p4/3P1B2/8/PPP1PPPP/R2QKBNR b KQkq - 0 1");  /// Input a FEN_string here!

    // Extract info from FEN string
    FEN tokens = extract_fen_tokens(board_fen);

#if DEBUG
    render_all(tokens->BBoard);
    int score = evaluateMaterial(tokens->BBoard, tokens->whiteToMove);
    printf("Score: %d \n", score);
#endif

    /// Do AI stuff here;
    move bestMove = calloc(1, sizeof(struct move_info));
    AIMove(tokens, bestMove);

    printf("Before AI move - ");
    render_all(tokens->BBoard);

    make_move(tokens->BBoard, bestMove);

    printf("After AI move - ");
    render_all(tokens->BBoard);

    // Free pointers
    free(bestMove);
    free_tokens(tokens);
    free(board_fen);
    return 0;
}
