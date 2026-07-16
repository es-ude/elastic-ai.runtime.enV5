#ifndef I2C_MODULE_H
#define I2C_MODULE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/*
All addresses are 7bit unsigned integers.
*/

typedef struct I2cModule I2cModule;
typedef uint8_t I2cStatus;

void I2cModule_init(I2cModule *self);

int I2cModule_readBlocking(I2cModule *self, uint8_t addr, void *dst, size_t len);
int I2cModule_writeBlocking(I2cModule *self, uint8_t addr, const void *src, size_t len);

/**
 * Read from a register of the i2c slave identified by slave_addr.
 * This will first send the reg_addr to the slave and start reading
 * afterwards.
 */
int I2cModule_readFromRegBlocking(I2cModule *self, uint8_t slave_addr, const void *reg_addr,
                                  uint8_t reg_addr_len, void *dst, size_t len);

int I2cModule_writeToRegBlocking(I2cModule *self, uint8_t slave_addr, const void *reg_addr,
                                 uint8_t reg_addr_len, const void *src, size_t len);

bool I2cModule_isInitialized(I2cModule *self);

/*get the error code from last I2cModule call*/
I2cStatus I2cModule_getError(I2cModule *self);

/*last call to I2cModule did not produce any errors*/
bool I2cModule_success(I2cModule *self);

enum {
    I2C_NO_ERROR,
    I2C_ERROR,
};
#endif
