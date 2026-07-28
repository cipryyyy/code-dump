/**
 @file Hamming.c
 @brief Encode and decode with SECDED Hamming(32, 26) algorithm
 @note Package composed of 26 data bits and 5 parity checks (5 bits SEC, bit0 for DED)
 @todo REMOVE malloc, add DED, whole lotta things
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PARITY_BITS     6   //5 SEC + 1 DED
#define DATA_BITS       26

static inline uint32_t leftCircularShift(uint32_t src, uint8_t shift) {
    if ((shift % 32) == 0) return src;
    return ((src << (shift % 32)) | (src >> (32 - (shift % 32))));
}

static inline uint16_t pow(uint16_t base, uint16_t exp) {
    uint16_t ret = 1;
    for (int i = 0; i < exp; i++) {
        ret *= base;
    }
    return ret;
}

static inline uint16_t hammingLength(uint16_t packageLength) {
    return (((packageLength * 8) + 25) / 26) * 4;
}

uint8_t* encodeHamming(uint8_t* package, uint16_t packageLength) {
    uint16_t mallocSize = hammingLength(packageLength);

    uint8_t* hamming = (uint8_t*)malloc(mallocSize);
    uint32_t rawPackage = 0;

    uint32_t hammingPointer = 0;
    uint32_t packagePointer = 0;
    uint32_t packageMask = 0x3FFF;

    //Loopy calc
    for (int i = 0; i < mallocSize / 4; i++) {
        uint32_t hammingEncodedPackage = 0;     //Encoded package
        uint8_t parityAccumulator[6] = {0};      //Accumulator for parity calculations (overflow is not a problem in here)

        memcpy(&rawPackage, &package[packagePointer], 4);
        //
        rawPackage &= packageMask;                              //Select only last 26 bits
        leftCircularShift(packageMask, DATA_BITS);  //Fix mask for the next run

        //Prepare the package structure (hardcoded masks cause yes)
        //               1    //1
        //            1110    //1 + 3
        //       111110000    //1 + 3 + 5
        //1111111000000000    //1 + 3 + 5 + 7

        hammingEncodedPackage |= ((rawPackage & 0x1UL) << 3);               //3
        hammingEncodedPackage |= ((rawPackage & 0xEUL) << (5 - 1));         //4
        hammingEncodedPackage |= ((rawPackage & 0x7F0UL) << (9 - 4));       //5
        hammingEncodedPackage |= ((rawPackage & 0x3FFF800UL) << (17 - 11)); //6

        for (int i = 1; i < 32; i++) {
            if (!((hammingEncodedPackage >> i) & 1UL)) continue;

            for (int i = 1; i < PARITY_BITS; i++) {
                if (i & pow(2, (i - 1)))  parityAccumulator[i]++;
            }
        }

        for (int i = 1; i < PARITY_BITS; i++) {
            hammingEncodedPackage |= ((uint32_t)(parityAccumulator[i] % 2) << pow(2, i));
        }

        memcpy(&hamming[hammingPointer], (void*)&hammingEncodedPackage, 4);

        packagePointer += 4;
        hammingPointer += 4;
    }

    return hamming;
}

int decodeHamming(uint8_t* package) {
    return 0;
}

int main() {
    uint8_t package[] = "lorem ipsum";
    uint8_t* hammingPackage = encodeHamming(package, sizeof(package));
    for (int i = 0; i < hammingLength(sizeof(package)); i++) {
        for (int j = 0; j < 8; j++){
            printf("%d", (hammingPackage[i] >> j) & 0x1);
        }
        printf("\n");
    }
}
