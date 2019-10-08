#include <stdint.h>

#include "i2c-drv.h"
#include "at24c02.h"


// 0xA0>>1=0x50
#define DEV_ADDR_AT24C02         0x50


uint8_t at24c02_read(uint8_t address)
{
    uint8_t value;

    i2c_nbytes_read(DEV_ADDR_AT24C02, address, &value, 1);

    return value;
}


void at24c02_write(uint8_t address, uint8_t value)
{
    i2c_byte_write(DEV_ADDR_AT24C02, address, value);
}


