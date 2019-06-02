#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define random_1(a,b) ((rand()%(b-a))+a)    // [a, b)
#define random_2(a,b) ((rand()%(b-a+1))+a)  // [a, b]

#define BUF_SIZE        7
#define I2C_DEVICE      "/dev/at24cxx"


unsigned char at24cxx_read(unsigned char address)
{
    int fd;
    unsigned char buf[1];

    buf[0] = address;

    fd = open(I2C_DEVICE, O_RDWR | O_NDELAY);
    read(fd, buf, 1);

    return buf[0];
}

void at24cxx_write(unsigned char address, unsigned char value)
{
    int fd;
    unsigned char buf[2];

    buf[0] = address;
    buf[1] = value;

    fd = open(I2C_DEVICE, O_RDWR | O_NDELAY);
    write(fd, buf, 2);
}

void print_buf(const char *tips, unsigned char *buf, unsigned char len)
{
    int i;

    printf("%s:\n", tips);
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

int main(int argc, char **argv)
{
    int i;
    unsigned char rbuf[7] = {0};
    unsigned char wbuf[7] = {0x20, 0x19, 0x06, 0x01, 0x18, 0x10, 0x17};

    srand(time(0));

    for (i = 0; i < 7; i++) {
        rbuf[i] = at24cxx_read(i);
    }
    print_buf("rbuf", rbuf, 7);

    if (wbuf[0] == rbuf[0]) {
        wbuf[6] += random_2(0, 0xFF);
    }

    for (i = 0; i < 7; i++) {
        at24cxx_write(i, wbuf[i]);
    }
    print_buf("wbuf", wbuf, 7);

    for (i = 0; i < 7; i++) {
        rbuf[i] = at24cxx_read(i);
    }
    print_buf("rbuf", rbuf, 7);

    return 0;
}


