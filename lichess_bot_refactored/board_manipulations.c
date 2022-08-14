//
// Created by Casper Wong on 6/19/22.
//

#include <stdint.h>
#include <stdio.h>
#include "dataStructs.h"
#include "lib/contracts.h"

enum enumSquare bitScanForward(uint64_t bb) {
    REQUIRES(bb != 0);
    const uint64_t debruijn64 = 0x03f79d71b4cb0a89;
    return LS1Bindex64[((bb ^ (bb-1)) * debruijn64) >> 58];
}


enum enumSquare bitScanReverse(uint64_t bb) {
    const uint64_t debruijn64 = 0x03f79d71b4cb0a89;
    REQUIRES(bb != 0);
    bb |= bb >> 1;
    bb |= bb >> 2;
    bb |= bb >> 4;
    bb |= bb >> 8;
    bb |= bb >> 16;
    bb |= bb >> 32;
    return LS1Bindex64[(bb * debruijn64) >> 58];
}


int popCount(uint64_t bb) {
    int count = 0;
    while (bb) {
        count++;
        bb &= bb - 1; // reset LS1B
    }
    return count;
}


uint64_t rankMask(enum enumSquare sq) {return 0xffUL << (sq & 56);}

uint64_t fileMask(enum enumSquare sq) {return 0x0101010101010101 << (sq & 7);}

uint64_t diagonalMask(enum enumSquare sq) {
    const uint64_t diagonal = 0x8040201008040201;
    int diag = 8 * (sq & 7) - (sq & 56);
    int north = -diag & (diag >> 31);
    int south = diag & (-diag >> 31);
    return (diagonal >> south) << north;
}

uint64_t antiDiagMask(enum enumSquare sq) {
    const uint64_t antiDiagonal = 0x0102040810204080;
    int diag =56- 8 * (sq & 7) - (sq & 56);
    int north = -diag & (diag >> 31);
    int south = diag & (-diag >> 31);
    return (antiDiagonal >> south) << north;
}


uint64_t northRay(enum enumSquare sq) {return fileMask(sq) & (-2UL << sq);}

uint64_t southRay(enum enumSquare sq) {return fileMask(sq) & (1UL << sq) - 1;}

uint64_t eastRay(enum enumSquare sq) {return rankMask(sq) & (-2UL << sq);}

uint64_t westRay(enum enumSquare sq) {return rankMask(sq) & (1UL << sq) - 1;}

uint64_t northEastRay(enum enumSquare sq) {return diagonalMask(sq) & (-2UL << sq);}

uint64_t southWestRay(enum enumSquare sq) {return diagonalMask(sq) & (1UL << sq) - 1;}

uint64_t northWestRay(enum enumSquare sq) {return antiDiagMask(sq) & (-2UL << sq);}

uint64_t southEastRay(enum enumSquare sq) {return antiDiagMask(sq) & (1UL << sq) - 1;}