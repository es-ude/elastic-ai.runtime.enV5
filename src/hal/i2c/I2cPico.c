#include "eai/hal2/I2cPico.h"
#include "eai/hal2/I2c.h"
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <pico/error.h>

static const uint8_t STATUS_INITIALIZED = 0x80;

static inline void _set_error(I2cModule *self, uint8_t error) {
  self->status |= (~STATUS_INITIALIZED & error);
}
static inline void _clear_error(I2cModule *self, uint8_t error) {
  self->status &= (STATUS_INITIALIZED | error);
}
static inline uint8_t _get_error(I2cModule *self) { return self->status & (~STATUS_INITIALIZED); }
static inline bool _status_ok(I2cModule *self) { return _get_error(self) == I2C_NO_ERROR; }

static inline int _write(I2cModule *self, const void *src, uint8_t slave_addr, size_t len,
                         bool nostop) {
  int bytes_written = i2c_write_blocking(self->i2c_mod, slave_addr, src, len, false);
  if (bytes_written == PICO_ERROR_GENERIC || bytes_written != len) {
    _set_error(self, I2C_ERROR);
    bytes_written = 0;
  }
  return bytes_written;
}

static inline int _read(I2cModule *self, void *dst, uint8_t slave_addr, size_t len) {
  int bytes_read = i2c_read_blocking(self->i2c_mod, slave_addr, dst, len, false);
  if (bytes_read == PICO_ERROR_GENERIC || bytes_read != len) {
    _set_error(self, I2C_ERROR);
    bytes_read = 0;
  }
  return bytes_read;
}

void I2cModule_init(I2cModule *self) {
  i2c_init(self->i2c_mod, self->freq_khz * 1000);

  gpio_set_function(self->pin_sda, GPIO_FUNC_I2C);
  gpio_set_function(self->pin_scl, GPIO_FUNC_I2C);
  gpio_pull_up(self->pin_sda);
  gpio_pull_up(self->pin_scl);
  self->status = STATUS_INITIALIZED;
}

bool I2cModule_wasInitialized(I2cModule *self) {
  return (self->status & STATUS_INITIALIZED) != 0x00;
}

int I2cModule_readBlocking(I2cModule *self, uint8_t addr, void *dst, size_t len) {
  return _read(self, dst, addr, len);
}

int I2cModule_writeBlocking(I2cModule *self, uint8_t addr, const void *src, size_t len) {
  return _write(self, src, addr, len, false);
}

int I2cModule_readFromRegBlocking(I2cModule *self, uint8_t slave_addr, const void *reg_addr,
                                  uint8_t reg_addr_len, void *dst, size_t len) {
  _write(self, reg_addr, slave_addr, len, true);
  if (!_status_ok(self)) {
    return 0;
  }
  sleep_us(10);
  return _read(self, dst, slave_addr, len);
}

int I2cModule_writeToRegBlocking(I2cModule *self, uint8_t slave_addr, const void *reg_addr,
                                 uint8_t reg_addr_len, const void *src, size_t len) {
  _write(self, reg_addr, slave_addr, len, true);
  if (!_status_ok(self)) {
    return 0;
  }
  sleep_us(10);
  return _write(self, reg_addr, slave_addr, len, false);
}

uint8_t I2cModule_getError(I2cModule *self) { return _get_error(self); }
bool I2cModule_success(I2cModule *self) { return _status_ok(self); }
