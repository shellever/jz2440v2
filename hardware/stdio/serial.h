#ifndef __SERIAL_H__
#define __SERIAL_H__

#define SERIAL_ECHO

void uart0_init(void);
void putc(unsigned char c);
unsigned char getc(void);

#endif // __SERIAL_H__
