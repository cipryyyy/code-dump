#include <stdint.h>

// Base addresses
#define RCC_BASE        0x40023800      //(section 2.3)
#define GPIOA_BASE      0x40020000      //(section 2.3)

// Clock enable
#define RCC_AHB1ENR     *(volatile uint32_t *)(RCC_BASE + 0x30)         //(section 6.3.9)

//GPIOA config
#define GPIOA_MODER     *(volatile uint32_t *)(GPIOA_BASE + 0x00)       //pin mode      (section 8.4.1)
#define GPIOA_ODR       *(volatile uint32_t *)(GPIOA_BASE + 0x14)       //pin value     (section 8.4.6)

#define RCC_GPIOAEN     (1 << 0)    //Enable GPIOA clock (section 6.3.22)
#define GPIOA5          (1 << 5)    //PA7 bit (section 8.4.11)

void main(void) {
    RCC_AHB1ENR |= RCC_GPIOAEN;     //Enable GPIOA clock

    //Set pin PA7 as output (0x01) in MODER register
    GPIOA_MODER &= ~(0b11UL << (5 * 2));    //Clear bits
    GPIOA_MODER |= (0b01UL << (5 * 2));     //Set as output

    while(1) {
        GPIOA_ODR |= GPIOA5;    //Set pin HIGH
        for (int i = 0; i < 500000; i++) {
            //wait
        }
        GPIOA_ODR &= ~GPIOA5;    //Set pin LOW
        for (int i = 0; i < 500000; i++) {
            //wait
        }
    }
}