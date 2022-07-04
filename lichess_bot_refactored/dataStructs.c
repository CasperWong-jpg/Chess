#include <stdint.h>
#include <stdio.h>

uint64_t not_a_file = 0xfefefefefefefefe;
uint64_t not_ab_file = 0xfcfcfcfcfcfcfcfc;
uint64_t not_h_file = 0x7f7f7f7f7f7f7f7f;
uint64_t not_hg_file = 0x3f3f3f3f3f3f3f3f;

const int LS1Bindex64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
    57, 49, 41, 37, 28, 16,  3, 61,
    54, 58, 35, 52, 50, 42, 21, 44,
    38, 32, 29, 23, 17, 11,  4, 62,
    46, 55, 26, 59, 40, 36, 15, 53,
    34, 51, 20, 43, 31, 22, 10, 45,
    25, 39, 14, 33, 19, 30,  9, 24,
    13, 18,  8, 12,  7,  6,  5, 63
};
