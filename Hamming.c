/**
 @file Hamming.c
 @brief Encode and decode with SECDED Hamming(32, 26) algorithm
 @note Package composed of 26 data bits and 5 parity checks (5 bits SEC, bit0 for DED)
 @todo REMOVE malloc for embed application
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @fn hammingLength
 * @brief returns number of bytes used after encoding
 * @note goes with increment of four since the coding is hamming (32, 26)
 * @param packageLength data package size in bytes
 * @return uint16_t number of bytes (uint8_t) used
*/
static inline uint16_t hammingLength(uint16_t packageLength) {
    return (((packageLength * 8) + 25) / 26) * 4;
}

/**
 * @fn revertHammingLength
 * @brief returns number of bytes used after decoding
 * @note If parameter isn't a multiple of 4 it cracks, but idc
 * @param hammingPackageLength encoded package length
 * @return uint16_t number of bytes (uint8_t) used
*/
static inline uint16_t revertHammingLength(uint16_t hammingPackageLength) {
    return ((((hammingPackageLength / 4) * 26) - 25) / 8);
}

/**
 * @fn encodeHamming
 * @brief Encoder for hamming(32, 26)
 * @param package package with the uint8_t data
 * @param packageLength package size in bytes
 * @return uint8_t* Pointer to the encoded array (size can be calculated with hammingLength())
*/
uint8_t* encodeHamming(uint8_t* package, uint16_t packageLength) {
    uint16_t mallocSize = hammingLength(packageLength);
    uint8_t* hamming = (uint8_t*)calloc(1, mallocSize);

    uint16_t blockIndex = 0;                  //Shitty idea to operate with bits

    for (int i = 0; i < mallocSize / 4; i++) {
        //Init
        uint32_t rawPackage = 0;                //Package with 26 byte data (+5 final bitsthat we will be ignored)
        uint32_t hammingEncodedPackage = 0;     //Encoded package
        uint8_t parityAccumulator[6] = {0};

        //Prepare package
        uint16_t byteIndex = (blockIndex * 26) / 8;                      //Where current bit is located in the byte struct
        uint16_t byteOffset = (blockIndex * 26) % 8;                     //Offset in it

        for (int i = 0; i < 5; i++) {
            if (byteIndex + i < packageLength) {
                rawPackage |= ((uint32_t)package[byteIndex + i]) << (8 * i);
            }
        }
        rawPackage >>= byteOffset;                              //Fix offset
        rawPackage &= 0x03FFFFFF;

        //Hamming code structure
        hammingEncodedPackage |= ((rawPackage & 0x1UL) << 3);
        hammingEncodedPackage |= ((rawPackage & 0xEUL) << 4);
        hammingEncodedPackage |= ((rawPackage & 0x7F0UL) << 5);
        hammingEncodedPackage |= ((rawPackage & 0x3FFF800UL) << 6);

        //Parity checks
        for (int i = 1; i < 32; i++) {
            if (!((hammingEncodedPackage >> i) & 1)) continue;      //bit = 0, skip
            for (int parityBit = 0; parityBit < 5; parityBit++) {
                if ((i >> parityBit) & 1) parityAccumulator[parityBit + 1]++;       //Same here
            }
        }

        //Parity bit placement
        for (int i = 0; i < 5; i++) {
            hammingEncodedPackage |= ((parityAccumulator[i + 1] & 1) << (1 << i));
        }
        //Global parity check
        for (int i = 1; i < 32; i++) {
            if (!((hammingEncodedPackage >> i) & 1)) continue;      //bit = 0, skip
            parityAccumulator[0]++;
        }
        hammingEncodedPackage |= ((parityAccumulator[0] & 1));

        //Copy value in return array
        memcpy(&hamming[blockIndex * 4], &hammingEncodedPackage, sizeof(uint32_t));
        blockIndex++;
    }
    return hamming;
}

uint8_t* decodeHamming(uint8_t* hammingPackage, uint16_t packageLength) {
    uint16_t callocSize = revertHammingLength(packageLength);
    uint8_t* package = (uint8_t*)calloc(1, callocSize);
    uint16_t blockIndex = 0;
    
    for (int i = 0; i < packageLength / 4; i++) {
        uint32_t rawPackage = 0;
        uint32_t dataPackage = 0;
        memcpy(&rawPackage, &hammingPackage[blockIndex * 4], sizeof(uint32_t));

        uint8_t SECtarget = 0;
        uint8_t globalParity = 0;

        //Error checking
        for (int i = 1; i < 32; i++) {
            if ((rawPackage >> i) & 1) {
                SECtarget ^= i;
            }
        }

        //Check global parity
        for (int i = 0; i < 32; i++) {
            if ((rawPackage >> i) & 1) {
                globalParity ^= 1;
            }
        }

        if (SECtarget != 0) {
            if (globalParity != 0) {
                // SEC, flip affected bit
                rawPackage ^= (1UL << SECtarget);
            } else {
                // DED, skip package
                continue; 
            }
        }

        //ᓚᘏᗢᓚᘏᗢᓚᘏᗢᓚᘏᗢᓚᘏᗢᓚᘏᗢᓚᘏᗢ
        //Package prep
        uint16_t byteIndex = (blockIndex * 26) / 8;
        uint16_t byteOffset = (blockIndex * 26) % 8;

        uint64_t writeData = ((uint64_t)dataPackage) << byteOffset;

        for (int j = 0; j < 5; j++) {
            if (byteIndex + j < callocSize) {
                package[byteIndex + j] |= (uint8_t)((writeData >> (8 * j)) & 0xFF);
            }
        }
        blockIndex++;
    }
    return package;
}

int main() {
    uint8_t package[] = "lorem ipsum";
    uint16_t packageSize = sizeof(package);

    uint16_t encodedSize = hammingLength(packageSize);
    uint8_t* hammingPackage = encodeHamming(package, packageSize);

    //TODO Apply bit flip to random packages

    uint8_t* decoded = decodeHamming(hammingPackage, encodedSize);

    for(int i = 0; i < packageSize; i++) {
        putchar(decoded[i]);
    }
    putchar('\n');

    free(hammingPackage);
    free(decoded);

    return 0;
}
