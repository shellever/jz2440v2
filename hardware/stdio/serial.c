#include "s3c24xx.h"
#include "serial.h"

#define TXD0READY       (1<<2)
#define RXD0READY       (1)

#define PCLK            50000000    // init.c中的clock_init函数设置PCLK为50MHz
#define UART_CLK        PCLK        // UART0的时钟源设为PCLK
#define UART_BAUD_RATE  115200      // 波特率
#define UART_BRD        ((UART_CLK / (UART_BAUD_RATE * 16)) - 1)

/*
 * 初始化UART0
 * 115200,8N1,无流控
 */
void uart0_init(void)
{
    GPHCON  |= 0xa0;    // GPH2,GPH3用作TXD0,RXD0
    GPHUP   = 0x0c;     // GPH2,GPH3内部上拉

    ULCON0  = 0x03;     // 8N1(8个数据位，无校验位，1个停止位)
    UCON0   = 0x05;     // 查询方式，UART时钟源为PCLK
    UFCON0  = 0x00;     // 不使用FIFO
    UMCON0  = 0x00;     // 不使用流控
    UBRDIV0 = UART_BRD; // 波特率为115200
}

void putc(unsigned char c)
{
	while(!(UTRSTAT0 & TXD0READY));
	UTXH0 = c;
}

unsigned char getc(void)
{
#ifdef SERIAL_ECHO
	unsigned char ret;
#endif	
	while(!(UTRSTAT0 & RXD0READY));
	ret = URXH0;
#ifdef SERIAL_ECHO	
	if (ret == 0x0d || ret == 0x0a) {
		putc(0x0d);
		putc(0x0a);
	} else {
		putc(ret);		
	}
#endif	
	return ret;
}


