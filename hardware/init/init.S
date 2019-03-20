@***********************************************
@ File：init.S
@ Desc：init sdram
@*********************************************** 
 
.text
.global _start
_start:
	// disable watchdog
	ldr r0, =0x53000000     @ watchdog base address
	mov r1, #0
	str r1, [r0]            @ r1 -> [r0]
	
	// disable interrupt
	ldr	r0, =0x4A000008     @ interrupt mask register
	mov	r1, #0xFFFFFFFF
	str	r1, [r0]
	
	ldr	r0, =0x4A00001C     @ interrupt sub maks register
	ldr	r1, =0x3FF
	str	r1, [r0]
	
	// init sdram
	mov r0, #0x48000000     @ memory controller base address
	adrl r1, mem_cfg_val    @ memory configure value
	mov r4, #0
1:	
	ldr r3, [r1], #4        @ r3 <- [r1], r1 += 4
	str r3, [r0], #4        @ r3 -> [r0], r0 += 4
	add r4, r4, #1          @ r4 += 1
	cmp r4, #13             @ r4 vs 13
	bne 1b                  @ if not equal, go back for next time

    // turn on led D10
    ldr r0, =0x56000050     @ GPFCON register address
    mov r1, #0x00000100     @ GPF4[9:8], 01=Output  
    str r1, [r0]            @ *rO = r1 
    ldr r0, =0x56000054     @ GPFDAT register address
    mov r1, #0x00000000     @ GPF[4], 1=off, 0=on
    str r1, [r0]            @ *rO = r1
 
loop:
	b loop

.align 4
mem_cfg_val:
    .long 0x22011110        @ BWSCON
    .long 0x00000700        @ BANKCON0
    .long 0x00000700        @ BANKCON1
    .long 0x00000700        @ BANKCON2
    .long 0x00000700        @ BANKCON3  
    .long 0x00000700        @ BANKCON4
    .long 0x00000700        @ BANKCON5
    .long 0x00018005        @ BANKCON6
    .long 0x00018005        @ BANKCON7
    .long 0x008C07A3        @ REFRESH
    .long 0x000000B1        @ BANKSIZE
    .long 0x00000030        @ MRSRB6
    .long 0x00000030        @ MRSRB7


