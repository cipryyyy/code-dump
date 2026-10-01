#include <stdint.h>
#include <stdio.h>

static void byteInvert(uint32_t input, uint32_t* output, int size)
{
    if (output == NULL)
        return;
    if (size > sizeof(uint32_t))
        return;

    uint32_t aux = 0;

    for (int i = 0; i < size; i++)
    {
        uint8_t byte = (input >> (i * 8)) & 0xFF;
        byte = ((byte >> 4) & 0x0F) | ((byte & 0x0F) << 4);
        aux |= (byte << ((sizeof(uint32_t) - sizeof(uint8_t) - i) * 8));
    }

    uint32_t mask = 0x0;
    for (int i = 0; i < size; i++)
        mask |= 0xFF << (8 * i);

    *output &= ~mask;
    *output |= aux >> (8 * (sizeof(uint32_t) - size));
}

int main(void)
{
    uint32_t input = 0xABCDEF01;
    uint32_t output;

    byteInvert(input, &output, sizeof(output));

    printf("I:%.8X\r\nO:%.8X\r\n", input, output);

    return 0;
}