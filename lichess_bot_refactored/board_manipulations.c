//
// Created by Casper Wong on 6/19/22.
//

#include <stdint.h>
#include <stdio.h>
#include "dataStructs.h"
#include "lib/contracts.h"

enum enumSquare bitScanForward(uint64_t bb) {
    REQUIRES(bb != 0);
    const uint64_t debruijn64 = (0x03f79d71b4cb0a89);
    return LS1Bindex64[((bb ^ (bb-1)) * debruijn64) >> 58];
}

int popCount(uint64_t bb) {
    int count = 0;
    while (bb) {
        count++;
        bb &= bb - 1; // reset LS1B
    }
    return count;
}


uint64_t rankMask(char sq) {return 0xff << (sq & 56);}

uint64_t fileMask(char sq) {return 0x0101010101010101 << (sq & 7);}

uint64_t diagonalMask(char sq) {
    const uint64_t diagonal = 0x8040201008040201;
    int diag = 8 * (sq & 7) - (sq & 56);
    int north = -diag & (diag >> 31);
    int south = diag & (-diag >> 31);
    return (diagonal >> south) << north;
}

uint64_t antiDiagMask(char sq) {
    const uint64_t antiDiagonal = 0x0102040810204080;
    int diag =56- 8 * (sq & 7) - (sq & 56);
    int north = -diag & (diag >> 31);
    int south = diag & (-diag >> 31);
    return (antiDiagonal >> south) << north;
}