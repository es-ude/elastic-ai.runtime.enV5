#ifndef ENV5_QXI_HEADER
#define ENV5_QXI_HEADER

#include <stdint.h>

#include "hardware/spi.h"

/*! Below are the PIOs to the FPGA, depends on the hardware design of the ENv5:
 *
 * | RP2040 |             | FPGA     |
 * | -----: | :---------: | :------- |
 * |     16 | <--  RX --- | SPI_MISO |
 * |     17 | ---  CS --> | SPI_CS   |
 * |     18 | --- SCK --> | SPI_TX   |
 * |     19 | <--  TX --- | SPI_MOSI |
 */
#define QXI_SPI spi0
#define QXI_SPI_RX_PIN 16 // MISO
#define QXI_SPI_TX_PIN 19 // MOSI
#define QXI_SPI_SCK_PIN 18
#define QXI_SPI_CS_PIN 17

//! Baudrate@62MHz
#define QXI_BAUD_RATE (62000 * 1000)

//! The format depends on the design of the FPGA
#define QXI_CPOL SPI_CPOL_0
#define QXI_CPHA SPI_CPHA_1
#define QXI_ORDER SPI_MSB_FIRST

#define QXI_READ_COMMAND 0x40
#define QXI_WRITE_COMMAND 0x80

/*!
 * @brief initializes the ports for the SPI interfaces
 */
void qxiInit(void);

/*!
 * @brief deinitializes the ports for the SPI interfaces
 */
void qxiDeinit(void);

void qxiSetSpeed(uint32_t baudrate);

/*!
 * @brief reads a byte array
 *
 * \IMPORTANT We highly recommend using Env5HwConfig.c
 *
 * @param startAddress: uint16_t startAddress, needed for command
 * @param dataBuffer: pointer to uint8_t, data that stores the received data
 */
void qxiReadBlocking(uint16_t startAddress, uint8_t *dataBuffer, size_t length);

/*!
 * @brief sends a byte array to slave
 *
 * \IMPORTANT We highly recommend using Env5HwConfig.c
 *
 * @param spiConfiguration: struct that contains the SPI configuration
 * @param command: pointer to data_t, command to be send
 * @param data: pointer to data_t, data to be send
 */
void qxiWriteBlocking(uint16_t startAddress, uint8_t *data, size_t length);

#endif // ENV5_QXI_HEADER
