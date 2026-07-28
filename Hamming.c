/**
 @file Hamming.c
 @brief Encode and decode with SECDED Hamming(32, 26) algorithm
 @note Package composed of 26 data bits and 5 parity checks (5 bits SEC, bit0 for DED)
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PARITY_BITS     6   //5 SEC + 1 DED
#define DATA_BITS       26

uint8_t* encodeHamming(uint8_t* package, uint16_t packageLength) {
    uint16_t mallocSize = (((packageLength * 8) + 25) / 26) * 4
    uint8_t* hamming = (uint8_t*)malloc(mallocSize);
    uint32_t hammingEncodedPackage = 0;     //Encoded package
    uint8_t parityAccumulator[6] = {0};      //Accumulator for parity calculations (overflow is not a problem in here)
    uint32_t RawPackage = 0;

    //Loopy calc
    //! for (int i = 0; i < mallocSize/4; i++) {
    memcpy(&RawPackage, package, 4); // Temp, gotta change it, looks like shit

    //Prepare the package structure
    hammingEncodedPackage |= ((RawPackage & 0x1UL) << 3);
    hammingEncodedPackage |= ((RawPackage & 0xEUL) << (5 - 1));
    hammingEncodedPackage |= ((RawPackage & 0x7F0UL) << (9 - 4));
    hammingEncodedPackage |= ((RawPackage & 0x3FFF800UL) << (17 - 11));

    for (int i = 0; i < 32; i++) {
        if ((hammingEncodedPackage >> i) & 1UL) {
            // Looks like shit pt.2
            if (i & 1)  parityAccumulator[1]++;
            if (i & 2)  parityAccumulator[2]++;
            if (i & 4)  parityAccumulator[3]++;
            if (i & 8)  parityAccumulator[4]++;
            if (i & 16) parityAccumulator[5]++;
        }
    }

    hammingEncodedPackage |= ((uint32_t)(parityAccumulator[1] % 2) << 1);
    hammingEncodedPackage |= ((uint32_t)(parityAccumulator[2] % 2) << 2);
    hammingEncodedPackage |= ((uint32_t)(parityAccumulator[3] % 2) << 4);
    hammingEncodedPackage |= ((uint32_t)(parityAccumulator[4] % 2) << 8);
    hammingEncodedPackage |= ((uint32_t)(parityAccumulator[5] % 2) << 16);

    for (int i = 0; i < 32; i++){
        printf("%lu", (RawPackage & (0x1UL << i)) >> i);
    }
    printf("\n");
    for (int i = 0; i < 32; i++){
        printf("%lu", (hammingEncodedPackage & (0x1UL << i)) >> i);
    }
    printf("\n");
    //! }

    return hamming;
}

int decodeHamming(uint8_t* package) {
    return 0;
}

int main() {
    uint8_t package[] = "mex";
    encodeHamming(package, sizeof(package));
}