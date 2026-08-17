#include "eai/hal2/I2cPico.h"
#include "hardware/i2c.h"

I2cModule *getI2cModule(void) {
  static I2cModule i2c = {
      .freq_khz = 400, .i2c_mod = I2C_INSTANCE(1), .pin_sda = 6, .pin_scl = 7, .status = 0};
  if (!I2cModule_isInitialized(&i2c)) {
    I2cModule_init(&i2c);
  }
  return &i2c;
}
