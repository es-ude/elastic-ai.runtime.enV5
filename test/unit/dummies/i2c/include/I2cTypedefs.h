#ifndef ENV5_I2C_TYPEDEFS
#define ENV5_I2C_TYPEDEFS

#include <stdint.h>

typedef struct i2c_inst i2c_inst_t;

enum {
    I2C_NO_ERROR = 0x00,
    I2C_INIT_ERROR = 0x10,
    I2C_ACK_ERROR = 0x13,
    I2C_TIMEOUT_ERROR = 0x14,
};
typedef uint8_t i2cErrorCode_t;

#endif /* ENV5_I2C_TYPEDEFS */
