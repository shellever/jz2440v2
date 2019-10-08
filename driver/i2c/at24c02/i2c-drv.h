#ifndef __I2C_DRV_H__
#define __I2C_DRV_H__


uint8_t i2c_byte_write(uint8_t addr, uint8_t reg, uint8_t val);
int i2c_nbytes_write(uint8_t addr, uint8_t reg, uint8_t *val, int len);
uint8_t i2c_byte_read(uint8_t addr, uint8_t *val);
uint8_t i2c_nbytes_read(uint8_t addr, uint8_t reg, uint8_t *val, int len);


#endif // __I2C_DRV_H__
