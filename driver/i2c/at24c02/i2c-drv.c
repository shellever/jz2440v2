#include <stdio.h>  
#include <stdint.h>
#include <fcntl.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <assert.h>  
#include <string.h>  
#include <sys/types.h>  
#include <sys/ioctl.h>  
#include <linux/types.h>  
#include <linux/i2c.h>  
#include <linux/i2c-dev.h>  


static const char *g_i2cdev = "/dev/i2c-0";


// not supported in linux-2.6.22.6
#if 0
#define I2C_DEFAULT_TIMEOUT     1
#define I2C_DEFAULT_RETRY       3

//
int i2c_set(uint32_t timeout, uint32_t retry)
{
    int fd;
    int ret = 0;

    fd = open(g_i2cdev, O_RDWR | O_NDELAY);
    if (fd < 0) {
        printf("open device %s fail\n", g_i2cdev);
        return -1;
    }

    if (ioctl(fd, I2C_DEFAULT_TIMEOUT, timeout ? timeout : I2C_DEFAULT_TIMEOUT) < 0) {
        return -2;
    }

    if (ioctl(fd, I2C_DEFAULT_RETRY, retry ? retry : I2C_DEFAULT_RETRY) < 0) {
        return -3;
    }

    return 0;
}
#endif      


// Byte Write
// Start + Address(W) + Register + Data + Stop
// addr - slave address
// reg  - register address
// val  - register value
uint8_t i2c_byte_write(uint8_t addr, uint8_t reg, uint8_t val)
{
    int fd;
    int ret = 0;
    uint8_t outbuf[2];
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages;

    fd = open(g_i2cdev, O_RDWR | O_NDELAY);
    if (fd < 0) {
        printf("open device %s fail\n", g_i2cdev);
        return -1;
    }

    packets.nmsgs = 1;
    packets.msgs = &messages;

    messages.addr = addr;
    messages.flags = 0;     // write
    messages.len = 2;       // reg(1) + val(1)
    outbuf[0] = reg;
    outbuf[1] = val;
    messages.buf = outbuf;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&packets);
    if (ret < 0) {
        printf("write error!\n");
        ret = -2;
    }

    return ret;
}


// Page Write
// Start + Address(W) + Register + Data(n) + Stop
// addr - slave address
// reg  - register address
// val  - register value buffer
// len  - buffer length
int i2c_nbytes_write(uint8_t addr, uint8_t reg, uint8_t *val, int len)
{
    int fd;
    int ret = 0;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages;
    int i;

    fd = open(g_i2cdev, O_RDWR | O_NDELAY);
    if (fd < 0) {
        printf("open device %s fail\n", g_i2cdev);
        return -1;
    }

    packets.nmsgs = 1;
    packets.msgs = &messages;

    messages.addr = addr;
    messages.flags = 0;         // write
    messages.len = len + 1;     // reg(1) + val(len)
    messages.buf = (uint8_t *)malloc(len+1);
    if (messages.buf == NULL) {
        ret = -2;
        goto err;
    }

    messages.buf[0] = reg;
    for (i = 0; i < len; i++) {
        messages.buf[i+1]=*val;
        val++;
    }

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&packets);
    if (ret < 0) {
        printf("write nbyte error!\n");
        return -3;
    }

err:
    free(messages.buf);

    return 0;
}


// Current Address Read
// Start + Address(R) + Data + Stop
// addr - slave address
// reg  - register address
uint8_t i2c_byte_read(uint8_t addr, uint8_t *val)
{
    int fd;
    int ret = 0;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages;

    fd = open(g_i2cdev, O_RDWR | O_NDELAY);
    if (fd < 0) {
        printf("open device %s fail\n", g_i2cdev);
        return -1;
    }

    packets.nmsgs = 1;
    packets.msgs = &messages;

    messages.addr = addr;
    messages.flags = I2C_M_RD;      // read
    messages.len = 1;               // reg(1)
    messages.buf = val;             //

    ret = ioctl (fd, I2C_RDWR, (unsigned long)&packets);
    if (ret < 0) {
        printf("read error!\n");
        ret = -1;
    }

    return ret;
}


// Random Read
// Start + Address(W) + Register + Start + Address(R) + Data(n) + Stop
// addr - slave address
// reg  - register address
// val  - register value buffer
// len  - buffer length
uint8_t i2c_nbytes_read(uint8_t addr, uint8_t reg, uint8_t *val, int len)
{
    int fd;
    int ret = 0;
    uint8_t outbuf;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];

    fd = open(g_i2cdev, O_RDWR | O_NDELAY);
    if (fd < 0) {
        printf("open device %s fail\n", g_i2cdev);
        return -1;
    }

    outbuf = reg;
    packets.nmsgs = 2;                  // 2 start signal

    messages[0].addr = addr;
    messages[0].flags = 0;              // write
    messages[0].len = 1;                // reg(1)
    messages[0].buf = &outbuf;          // reg

    messages[1].len = len;              //
    messages[1].addr = addr;            //
    messages[1].flags = I2C_M_RD;       // read
    messages[1].buf = val;

    packets.msgs = messages;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&packets);
    if (ret < 0) {
        printf("read nbyte error!\n");
        return -2;
    }

    return 0;
}


