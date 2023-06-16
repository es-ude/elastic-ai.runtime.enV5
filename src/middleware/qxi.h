#ifndef MY_PROJECT_QXI_H
#define MY_PROJECT_QXI_H

#include "hardware/spi.h"
#include "pico/stdlib.h"
#include <stdint.h>

// Below are the PIOs to the FPGA, depends on the hardware design
// on ENv5
// RP2040                  FPGA
// 16      <--  RX  ---   SPI_MISO
// 17      ---  CS  -->   SPI_CS
// 18      --- SCK  -->   SPI_TX
// 19      <--  TX  ---   SPI_MOSI
#define QXI_SPI spi0
#define QXI_SPI_RX_PIN 16 // MISO
#define QXI_SPI_TX_PIN 19 // MOSI
#define QXI_SPI_SCK_PIN 18
#define QXI_SPI_CS_PIN 17

// #define QXI_BAUD_RATE 62000*1000 // @62MHz
#define QXI_BAUD_RATE (62000 * 1000) // @62MHz

// Notice: the format is depends on the design on FPGA
#define QXI_CPOL SPI_CPOL_0
#define QXI_CPHA SPI_CPHA_1
#define QXI_ORDER SPI_MSB_FIRST

#define QXI_READ_COMMAND 0x40
#define QXI_WRITE_COMMAND 0x80

void qxi_init(void);
void qxi_deinit(void);
void qxi_read_blocking(uint16_t addr, uint8_t *buf, size_t len);
void qxi_write_blocking(uint16_t addr, uint8_t data[], uint16_t len);
void qxi_set_speed(uint baudrate);

#endif // MY_PROJECT_QXI_H
