@******************************************************************************
@ File:  led_on_s.S
@ Brief: turn on LED1 using assembly
@******************************************************************************       
            
.text
.global _start
_start:     
    LDR     R0, =0x56000050      @ GPFCON register address
    MOV     R1, #0x00000100      @ GPF4[9:8], 01=Output  
    STR     R1, [R0]             @ *RO = R1 
    LDR     R0, =0x56000054      @ GPFDAT register address
    MOV     R1, #0x00000000      @ GPF[4], 1=off, 0=on
    STR     R1, [R0]             @ *RO = R1
MAIN_LOOP:
    B       MAIN_LOOP

