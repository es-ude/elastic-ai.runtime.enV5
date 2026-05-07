#ifndef HAL_I2C_H_
#define HAL_I2C_H_


#include "pico/stdlib.h"
#include "hardware/i2c.h"


#ifndef PICO_DEFAULT_I2C_SDA_PIN
#define PICO_DEFAULT_I2C_SDA_PIN 6
#endif

#ifndef PICO_DEFAULT_I2C_SCL_PIN
#define PICO_DEFAULT_I2C_SCL_PIN 7
#endif

// ========================================= DEFINITIONS ===============================================
/*! \brief Struct handler for configuring the I2C interface of RP2040
* \param pin_sda        GPIO num of used Serial Data Line (SDA)
* \param pin_scl        GPIO num of used Serial Clock Line (SCL)
* \param i2c_mod        I2C handler of RP2040 (i2c0 or i2c1)
* \param fi2c_khz       Used I2C clock [in kHz]
* \param avai_devices   Number of available devices on bus
* \param init_done      Boolean if configuration is done
*/
typedef struct{
    uint8_t pin_sda;
    uint8_t pin_scl;
    i2c_inst_t *i2c_mod;
    uint16_t fi2c_khz;
    uint8_t avai_devices;
    bool init_done;
} i2c_rp2_t;

static i2c_rp2_t DEVICE_I2C_DEFAULT = {
    .pin_sda = PICO_DEFAULT_I2C_SDA_PIN,
    .pin_scl = PICO_DEFAULT_I2C_SCL_PIN,
    .i2c_mod = i2c0,
    .fi2c_khz = 100,
    .avai_devices = 0,
    .init_done = false
};


// ========================================= FUNCTIONS ===============================================
/*! \brief Function for initialising the I2C interface of RP2040
* \param handler    Pointer to struct for setting-up the I2C interface module
* \return   Bool if initialization of I2C module was successful
*/
bool init_i2c_module(i2c_rp2_t *handler);


/*! \brief Function for configuring the I2C interface of RP2040
* \param handler    Pointer to struct for setting-up the I2C interface module
* \return   Bool if configuration of I2C module was successful
*/
bool configure_i2c_module(i2c_rp2_t *handler);


/*! \brief Function for scanning the I2C bus if devices are available
* \param handler        Pointer to struct of I2C module
*/
void scan_i2c_bus_for_device(i2c_rp2_t *handler);


/*! \brief Function for scanning the I2C bus if devices are available. Perform a 1-byte dummy read from the probe address. If a slave
    acknowledges this address, the function returns the number of bytes transferred. If the address byte is ignored, the function returns -1 (=".").
* \param handler            Pointer to struct of I2C module
* \param addr               I2C address to search for device
*/
bool check_i2c_bus_for_device_specific(i2c_rp2_t *handler, uint8_t addr);


/*! \brief Function for scanning the I2C bus if devices are available. Perform a 1-byte dummy read from the probe address. If a slave
    acknowledges this address, the function returns the number of bytes transferred. If the address byte is ignored, the function returns -1 (=".").
* \param handler        Pointer to struct of I2C module
*/
bool check_i2c_bus_for_device_total(i2c_rp2_t *handler);


/*! \brief Function with RPi Pico constructor for writing on I2C bus
* \param i2c_handler    Pointer to struct of I2C module
* \param adr            Adress of selected I2C device
* \param buffer_tx      uint8 arrray with data to send on I2C bus
* \return               Boolean if transmission is completed
*/
bool construct_i2c_write_data(i2c_rp2_t *i2c_handler, uint8_t adr, uint8_t buffer_tx[], size_t len_tx);


/*! \brief Function with RPi Pico constructor for reading on I2C bus
* \param i2c_handler    Pointer to struct of I2C module
* \param adr            Adress of selected I2C device
* \param buffer_tx      uint8 arrray with data to send on I2C bus
* \param buffer_tx      uint8 arrray with data to receive from I2C bus
* \return               Boolean if transmission is completed and data is valid (call with processing buffer_rx directly)
*/
bool construct_i2c_read_data(i2c_rp2_t *i2c_handler, uint8_t adr, uint8_t buffer_tx[], size_t len_tx, uint8_t buffer_rx[], size_t len_rx);


/*! \brief Function with RPi Pico constructor for processing data from buffer
* \param buffer_tx      uint8 arrray with data to receive from I2C bus
* \param size           Length of array
* \return               Value
*/
uint64_t translate_array_into_uint64(uint8_t buffer_rx[], size_t len_rx);


#endif
