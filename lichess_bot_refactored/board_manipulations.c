//
// Created by Casper Wong on 6/19/22.
//

#include <stdint.h>
#include "dataStructs.h"
#include "lib/contracts.h"

int bitScanForward(uint64_t bb) {
    REQUIRES(bb != 0);
    const uint64_t debruijn64 = (0x03f79d71b4cb0a89);
    assert (bb != 0);
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

uint64_t flipVertical(uint64_t x) {
    const uint64_t k1 = 0x00FF00FF00FF00FF;
    const uint64_t k2 = 0x0000FFFF0000FFFF;
    x = ((x >>  8) & k1) | ((x & k1) <<  8);
    x = ((x >> 16) & k2) | ((x & k2) << 16);
    x = ( x >> 32)       | ( x       << 32);
    return x;
}