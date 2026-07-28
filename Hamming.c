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

static uint16_t pow(uint16_t base, uint16_t exp) {
    uint16_t ret = 1;
    for (int i = 0; i < exp; i++) {
        ret *= base;
    }
    return ret;
}

static uint16_t hammingLength(uint16_t packageLength) {
    return (((packageLength * 8) + 25) / 26) * 4;
}

uint8_t* encodeHamming(uint8_t* package, uint16_t packageLength) {
    uint16_t mallocSize = hammingLength(packageLength);
    uint8_t* hamming = (uint8_t*)malloc(mallocSize);
    uint32_t rawPackage = 0;
    uint32_t hammingPointer = 0;
    uint32_t packagePointer = 0;

    //Loopy calc
    for (int i = 0; i < mallocSize/4; i++) {
        uint32_t hammingEncodedPackage = 0;     //Encoded package
        uint8_t parityAccumulator[6] = {0};      //Accumulator for parity calculations (overflow is not a problem in here)

        memcpy(&rawPackage, &package[packagePointer], 4);
        rawPackage &= 0x3FFF;   //Select only last 26 bits

        //Prepare the package structure
        hammingEncodedPackage |= ((rawPackage & 0x1UL) << 3);
        hammingEncodedPackage |= ((rawPackage & 0xEUL) << (5 - 1));
        hammingEncodedPackage |= ((rawPackage & 0x7F0UL) << (9 - 4));
        hammingEncodedPackage |= ((rawPackage & 0x3FFF800UL) << (17 - 11));

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

        for (int i = 1; i < DATA_BITS; i++) {
            hammingEncodedPackage |= ((uint32_t)(parityAccumulator[i] % 2) << pow(2, i));
        }

        //printf("\nENC: ");
        for (int i = 0; i < 32; i++){
            if (i % 8 == 0) printf("\n");
            printf("%lu", (hammingEncodedPackage & (0x1UL << i)) >> i);
        }

        memcpy(&hamming[hammingPointer], (void*)&hammingEncodedPackage, 4);

        packagePointer += DATA_BITS;
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
    printf("\n\n");

    for (int i = 0; i < hammingLength(sizeof(package)); i++) {
        for (int j = 0; j < 8; j++){
            printf("%d", (hammingPackage[i] >> j) & 0x1);
        }
        printf("\n");
    }
}
