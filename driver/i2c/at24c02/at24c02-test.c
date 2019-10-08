#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "at24c02.h"

#define random_1(a,b) ((rand()%(b-a))+a)    // [a, b)
#define random_2(a,b) ((rand()%(b-a+1))+a)  // [a, b]

#define BUF_SIZE        7


void print_buf(const char *tips, unsigned char *buf, unsigned char len)
{
    int i;

    printf("%s: ", tips);
    for (i = 0; i < len; i++) {
        printf(" %02X", buf[i]);
    }
    printf("\n");
}


void print_usage(const char *prog)
{
    printf("\n");
    printf("Usage:\n");
    printf("%s\n", prog);
    printf("\n");
}


int main(int argc, char *argv[])
{
    int i;
    uint8_t rbuf[7] = {0};
    uint8_t wbuf[7] = {0x20, 0x19, 0x06, 0x01, 0x18, 0x10, 0x17};

    srand(time(0));

    for (i = 0; i < 7; i++) {
        rbuf[i] = at24c02_read(i);
    }
    print_buf("rbuf", rbuf, 7);

    if (wbuf[0] == rbuf[0]) {
        wbuf[6] += random_2(0, 0xFF);
    }

    for (i = 0; i < 7; i++) {
        at24c02_write(i, wbuf[i]);
    }
    print_buf("wbuf", wbuf, 7);

    for (i = 0; i < 7; i++) {
        rbuf[i] = at24c02_read(i);
    }
    print_buf("rbuf", rbuf, 7);

    return 0;
}


