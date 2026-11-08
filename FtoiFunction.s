/*
@file: ftoi.s
@brief cast a float input from memory into a integer
@input float in register r0
@output int32_t in register r0
@note Done with cpulator, ARMv7 generic
@note Number of basic instruction ~= 32
*/

.equ mantissaMASK, 0x7FFFFF
.equ exponentMASK, 0xFF
.equ exponentSHIFT, 23
.equ signSHIFT, 31
.equ zeroCHECK, 0x7FFFFFFF

.global _ftoi

.data
	n1: .float -12434.12345
	
.text
_ftoi:
	push {r1-r12}
	@LOAD VALUE in r0
	ldr r0, =n1
	ldr r0, [r0]
	
	@check if it is a zero
	ldr r9, =zeroCHECK
	and r9, r0, r9
	cmp r9, #0
	moveq r0, #0
	beq _end
	
	@EXTRACT MANTISSA in r1
	mov r1, r0
	ldr r10, =mantissaMASK
	and r1, r1, r10
	orr r1, #0x800000
	
	@EXTRACT ESPONENT in r2
	mov r2, r0
	ldr r10, =exponentMASK
	ldr r11, =exponentSHIFT
	lsr r2, r11
	and r2, r2, r10
	sub r2, #127
	
	@EXTRACT SIGN in r3
	mov r3, r0
	ldr r11, =signSHIFT
	lsr r3, r11
	
	@CALCULATE SHIFT in r4
	cmp r2, #23
	subge r2, r2, #23
	lslge r1, r2
	cmp r2, #0
	rsbge r2, r2, #23
	lsrge r1, r2
	movlt r1, #0
	
	@FIX SIGN
	mov r0, r1
	cmp r3, #0
	popeq {r1-r12}
	beq _end
	mvn r0, r0
	add r0, #1
	pop {r1-r12}
	b _end
