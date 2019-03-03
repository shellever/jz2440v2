#include "s3c24xx.h"

void EINT_Handle(void)
{
    unsigned long offset = INTOFFSET;
    
    // 通过INTOFFSET寄存器来获取中断类型
    switch (offset) {
        case 0:// S2被按下 
            GPFDAT |= (0x7<<4);   // 所有LED熄灭
            GPFDAT &= ~(1<<4);    // LED1点亮
            break;
        
        case 2:// S3被按下
            GPFDAT |= (0x7<<4);   // 所有LED熄灭
            GPFDAT &= ~(1<<5);    // LED2点亮
            break;

        case 5:// K4被按下
            GPFDAT |= (0x7<<4);   // 所有LED熄灭
            GPFDAT &= ~(1<<6);    // LED4点亮                
            break;
    }

    // 清中断
    if (offset == 5) 
        EINTPEND = (1<<11);     // EINT8_23合用IRQ5
    SRCPND = 1<<offset;
    INTPND = 1<<offset;
}


