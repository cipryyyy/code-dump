#include <stdio.h>
#include <stdint.h>

int mask_generator(int mask_size, int mask_offset, uint32_t* output)
{
    //init checks
    if (output == NULL)
        return -2;

    if ((mask_size + mask_offset) > (sizeof(*output) * 8))
        return -1;

    *output = ~(0x0);                                                 //ᓚᘏᗢ < Stupid fucking way to write 0xFFFF.... >
    *output >>= mask_offset;                                          // Discard right bits
    *output <<= (sizeof(*output) * 8) - mask_size;                    // Discard left bits
    *output >>= ((sizeof(*output) * 8) - mask_size) - mask_offset;    // Adjust centering

    return 0;
}

void print_bits(uint32_t num)
{
    printf("0b");

    for (int i = (sizeof(num) * 8) - 1; i >= 0; i--)
        printf("%lu", (num >> i) & 0x1UL);

    printf("\r\n");
}

int main(int argc, char* argv[])
{
    uint32_t res;

    if (mask_generator(9, 2, &res) != 0)
    {
        printf("Wrong setup for 32 bits mask\r\n");
        return 1;
    }
    
    print_bits(res);

    return 0;
}
