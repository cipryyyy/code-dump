/**
 * @file ftoi.c
 * @brief convert a float in integer
*/

#include <stdio.h>
#include <stdint.h>

int ftoi(float input) {
    uint32_t bitExtraction = *(uint32_t*)&input;

    if ((bitExtraction & 0x7FFFFFFF) == 0) return 0;

    uint32_t mantissa = (bitExtraction & 0x7FFFFF) | (1U << 23);
    int8_t exp = ((bitExtraction >> 23) & 0xFF) - 127;
    int8_t sign = 1 - 2*((bitExtraction >> 31) & 0x1);

    if (exp >= 23) {
        return sign * (mantissa << (exp - 23));
    } else if (exp >= 0) {
        return sign* (mantissa >> (23 - exp));
    }

    return 0;
}

int main(void) {
    float input = 124589.434212;
    printf("Input: %f\nOutput: %d\n", input, ftoi(input));
    return 0;
}
