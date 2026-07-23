.cpu cortex-m4
.thumb

.equ RCC_AHB1ENR,   0x40023830
.equ GPIOA_MODER,   0x40020000
.equ GPIOA_ODR,     0x40020014

.word 0x20020000
.word _reset
.thumb_func
_reset:
    ldr r0, =RCC_AHB1ENR
    mov r1, #1
    str r1, [r0]

    ldr r0, =GPIOA_MODER
    mov r1, #01
    lsl r1, #10
    str r1, [r0]

    ldr r0, =GPIOA_ODR
_loop:
    mov r1, #1
    lsl r1, #5
    str r1, [r0]
    ldr r2, =3000000
    bl _wait

    mov r1, #0
    lsl r1, #5
    str r1, [r0]
    ldr r2, =1000000
    bl _wait

    b _loop

_wait:
    sub r2, #1
    cmp r2, #0
    bne _wait
    mov pc, lr
