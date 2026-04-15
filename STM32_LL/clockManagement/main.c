#include <stdint.h>

//* Base addresses
#define RCC_BASE        0x40023800
#define GPIO_BASE       0x40020000
#define FLASH_BASE      0x40023C00

//GPIOx base address
#define GPIOA_BASE      (GPIO_BASE + 0x0000)
#define GPIOB_BASE      (GPIO_BASE + 0x0400)
#define GPIOC_BASE      (GPIO_BASE + 0x0800)
#define GPIOD_BASE      (GPIO_BASE + 0x0C00)

//* Registers
//Flash
#define FLASH_ACR       *(volatile uint32_t *)(FLASH_BASE + 0x00)   //Flash configuration

// GPIOA config
#define GPIOA_MODER     *(volatile uint32_t *)(GPIOA_BASE + 0x00)
#define GPIOA_ODR       *(volatile uint32_t *)(GPIOA_BASE + 0x14)

// Clock
#define RCC_AHB1ENR     *(volatile uint32_t *)(RCC_BASE + 0x30)
#define RCC_CR          *(volatile uint32_t *)(RCC_BASE + 0x00)     //Oscillator controller
#define RCC_PLLCFGR     *(volatile uint32_t *)(RCC_BASE + 0x04)     //PLL config
#define RCC_CFGR        *(volatile uint32_t *)(RCC_BASE + 0x08)     //RCC config

// Systick  (source arm.developer)
#define SYST_CSR    *(volatile uint32_t *)(0xE000E010) // Control and Status
#define SYST_RVR    *(volatile uint32_t *)(0xE000E014) // Reload Value
#define SYST_CVR    *(volatile uint32_t *)(0xE000E018) // Current Value
#define SYST_COUNT  16

//* BITS
//f_VCO = f_PLLinput * (PLLN / PLLM)
//f_clock = f_VCO / PLLP
#define PLLM_SHIFT      0
#define PLLN_SHIFT      6
#define PLLP_SHIFT      16
#define PLLM_MASK       0b111111UL 
#define PLLN_MASK       0b111111111UL
#define PLLP_MASK       0b11UL
#define PLLSRC_SHIFT    22

#define HSE_BYP         (1 << 18)   // Bypass, only for squared signals
#define HSE_ON          (1 << 16)   // Activate HSE
#define HSE_RDY         (1 << 17)   // HSE status

#define PLL_ON          (1 << 24)   // Activate PL
#define PLL_RDY         (1 << 25)   // PLL status

#define RCC_GPIOAEN     (1 << 0)
#define GPIOA5          (1 << 5)

//Helper functions
void delay(uint32_t ms) {
    SYST_RVR = 100000 - 1;  //1ms = 100'000 clock
    SYST_CVR = 0;           //Reset counter
    SYST_CSR = 0x05;        //Enable timer, source = processor, no INT

    for (uint32_t i = 0; i < ms; i++) {
        while (!(SYST_CSR & (1 << SYST_COUNT)));    //Counter reaches 0 for ms times
    }

    SYST_CSR = 0;           //Disable timer
}

void main(void) {
    RCC_CR |= HSE_BYP;      //Set bypass
    RCC_CR |= HSE_ON;       //Set as on

    while (!(RCC_CR & HSE_RDY)); //Wait for the clock to be ready

    //(table 5) Vcc =3.3V, HCLK = 100MHz -> 3WS
    FLASH_ACR &= ~(0b11111UL);
    FLASH_ACR |= 0x03;

    //Clock setup 8MHz / 8 (M) = 1MHz * 200 (N) = 200MHz / 2 (P) = 100MHz
    RCC_PLLCFGR &= ~((PLLM_MASK << PLLM_SHIFT) + (PLLN_MASK << PLLN_SHIFT) + (PLLP_MASK << PLLP_SHIFT));
    RCC_PLLCFGR |= (((8 & PLLM_MASK) << PLLM_SHIFT) | ((200 & PLLN_MASK) << PLLN_SHIFT) | ((0 & PLLP_MASK) << PLLP_SHIFT));

    //Set HSE as src
    RCC_PLLCFGR |= 1 << PLLSRC_SHIFT;

    //Turn on PLL
    RCC_CR |= PLL_ON;
    while (!(RCC_CR & PLL_RDY));    // Wait for it to be ready

    //Set PLL as system clock
    RCC_CFGR &= ~(0b11);    //Clear
    RCC_CFGR |= 0b10;

    //Wait for the switch (SWS)
    while (((RCC_CFGR >> 2) & 0x03) != 0b10);

    //blinker
    RCC_AHB1ENR |= RCC_GPIOAEN;

    GPIOA_MODER &= ~(0b11UL << (5 * 2));
    GPIOA_MODER |= (0b01UL << (5 * 2));

    while(1) {
        GPIOA_ODR |= GPIOA5;
        delay(500);
        GPIOA_ODR &= ~GPIOA5;
        delay(500);
    }
}