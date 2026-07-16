#include "eai/hal2/I2cPico.h"
#include "eai/hal2/I2c.h"
#include "hardware/gpio.h"

static inline int _write(I2cModule *self, uint8_t addr, const void *src, size_t len, bool nostop) {
  int bytes_written = i2c_write_blocking(self->i2c_mod, addr, src, len, nostop);
  if (bytes_written != len) {
    self->status |= 0x01;
  }
  return bytes_written;
}
static inline void _set_error(I2cModule *self, uint8_t error) { self->status |= (~0x80 & error); }

static inline void _clear_error(I2cModule *self, uint8_t error) { self->status &= (0x80 | error); }

static inline bool _status_ok(I2cModule *self) { return 0x08 & self->status == 0x00; }

void I2cModule_init(I2cModule *self) {
  i2c_init(self->i2c_mod, self->freq_khz * 1000);

  gpio_set_function(self->pin_sda, GPIO_FUNC_I2C);
  gpio_set_function(self->pin_scl, GPIO_FUNC_I2C);
  gpio_pull_up(self->pin_sda);
  gpio_pull_up(self->pin_scl);
  self->status = 0x80;
}

bool I2cModule_wasInitialized(I2cModule *self) { return self->status & 0x80; }

int I2cModule_readBlocking(I2cModule *self, uint8_t addr, void *dst, size_t len) {
  return i2c_read_blocking(self->i2c_mod, addr, dst, len, false);
}

int I2cModule_writeBlocking(I2cModule *self, uint8_t addr, const void *src, size_t len) {
  return i2c_write_blocking(self->i2c_mod, addr, src, len, false);
}

int I2cModule_readFromRegBlocking(I2cModule *self, uint8_t slave_addr, const void *reg_addr,
                                  uint8_t reg_addr_len, void *dst, size_t len) {
  int bytes_written = i2c_write_blocking(self->i2c_mod, slave_addr, reg_addr, reg_addr_len, true);
  if (bytes_written != reg_addr_len) {
    _set_error(self, I2C_ERROR);
    return 0;
  }
  sleep_us(10);
  int bytes_read = i2c_read_blocking(self->i2c_mod, slave_addr, dst, len, false);
  if (bytes_read != len) {
    _set_error(self, I2C_ERROR);
  }
  return bytes_read;
}

int I2cModule_writeToRegBlocking(I2cModule *self, uint8_t slave_addr, const void *reg_addr,
                                 uint8_t reg_addr_len, const void *src, size_t len) {
  int bytes_written = i2c_write_blocking(self->i2c_mod, slave_addr, reg_addr, reg_addr_len, true);
  if (bytes_written != reg_addr_len) {
    _set_error(self, I2C_ERROR);
    return 0;
  }
  sleep_us(10);
  int bytes_written = i2c_write_blocking(self->i2c_mod, slave_addr, src, len, false);
  if (bytes_written != len) {
    _set_error(self, I2C_ERROR);
  }
  return bytes_written;
}

uint8_t I2cModule_getError(I2cModule *self) { return self->status & (~0x80); }
bool I2cModule_success(I2cModule *self) { return I2cModule_getError(self) == 0x00; }
