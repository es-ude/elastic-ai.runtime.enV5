#ifndef PAC193X_H_
#define PAC193X_H_

#include "eai/hal/I2cPico.h"

// More details in datasheet (Table 5-1, p.26)
#define PAC193X_I2C_ADDR_START 0x10
#define PAC193X_I2C_ADDR_END   0x1F

#define PAC193X_RATE_1024SPS    0
#define PAC193X_RATE_256SPS     1
#define PAC193X_RATE_64SPS      2
#define PAC193X_RATE_8SPS       3

// More Informations in the datasheet at https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/PAC1931-Family-Data-Sheet-DS20005850E.pdf
/*! \brief Struct for configuring the current sensor module PAC193x from Microchip Technology
* \param spi                    Pointer to SPI interface
* \param gpio_pwrdwn            Power Down GPIO pin (not used = 255)
* \param gpio_alert             Alert GPIO pin (not used = 255)
* \param adr                    I2C address of the sensor
* \param num_channels           Number of channels to read (0: auto-detect, 1-4: selected)
* \param sample_rate            Integer for definign sampling rate (0=1024, 1=256, 2=64, 3=8 SPS)
* \param enable_channels        Flag for enabling channels (if false, all channels will be disabled, otherwise all channels will be enabled)
* \param enable_sleep_mode      Flag for enabling sleep mode (if true, the sensor will go to sleep mode after each measurement, otherwise it will stay in continuous measurement mode)
* \param enable_single_shot_mode Flag for enabling single shot mode (if true, the sensor will take a single measurement and then go back to sleep mode)
* \param enable_bipolar_voltage Flag for enabling bipolar voltage measurement (if true, the voltage output must be two complements, otherwise it is unsigned)
* \param enable_bipolar_current Flag for enabling bipolar current measurement (if true, the current output must be two complements, otherwise it is unsigned)
* \param init_done              Initialization done flag
*/
typedef struct {
    i2c_rp2_t *i2c;
    uint8_t gpio_pwrdwn;
    uint8_t gpio_alert;
    uint8_t adr;
    uint8_t num_channels;
    uint8_t sample_rate;
    bool enable_channels;
    bool enable_sleep_mode;
    bool enable_single_shot_mode;
    bool enable_bipolar_voltage;
    bool enable_bipolar_current;
    bool init_done;
} pac193x_t;

/*! \brief Function for getting the I2C address of the PAC193x sensor module (More details in datasheet [Table 5-1, p.26])
* \param resistor_value Value of the resistor connected to the ADR pin (0=GND, ..., 65535 = VDD)
* \return uint8_t with the I2C address (0x10 - 0x1F)
*/
uint8_t pac193x_get_i2c_address(uint32_t resistor_value);


/*! \brief Function for checking the product ID of the PAC193x sensor module
* \param self Pointer to the configuration struct
* \return true if product ID is correct (=0x58...0x5B), false otherwise
*/
bool pac193x_check_product_id(pac193x_t *self);


/*! \brief Function for checking the manufacturer ID of the PAC193x sensor module
* \param config Pointer to the configuration struct
* \return true if manufacturer ID is correct (=0x5D), false otherwise
*/
bool pac193x_check_manufacturer_id(pac193x_t *config);


/*! \brief Function for checking the revision ID of the PAC193x sensor module
* \param config             Pointer to the configuration struct
* \return true if revision ID is correct (=0x03), false otherwise
*/
bool pac193x_check_revision_id(pac193x_t *config);

/*! \brief Function for getting the number of active channels of the PAC193x sensor module
* \param config             Pointer to the configuration struct
* \return uint8_t with number of channels (1-4)
*/
uint8_t pac193x_get_number_of_channels(pac193x_t *config);


/*! \brief Function for defiing the single shot mode of the PAC193x sensor module
* \param config                     Pointer to the configuration struct
* \param enable_single_shot_mode    Flag for enabling single shot mode (if true, the sensor will take a single measurement and then go back to sleep mode)
* \return true if set successfully, false otherwise
*/
bool pac193x_set_single_shot_mode(pac193x_t *config, bool enable_single_shot_mode);


/*! \brief Function for defining the sleep mode of the PAC193x sensor module
* \param config             Pointer to the configuration struct
* \param enable_sleep_mode  Flag for enabling sleep mode (if true, the sensor will go to sleep mode after each measurement, otherwise it will stay in continuous measurement mode)
* \return true if set successfully, false otherwise
*/
bool pac193x_set_sleep_mode(pac193x_t *config, bool enable_sleep_mode);


/*! \brief Function for defining the sampling rate of the PAC193x sensor module
* \param config         Pointer to the configuration struct
* \param sample_rate    Integer for definign sampling rate (0=1024, 1=256, 2=64, 3=8 SPS)
* \return true if set successfully, false otherwise
*/
bool pac193x_set_sampling_rate(pac193x_t *config, uint8_t sample_rate);


/*! \brief Function for checking the revision ID of the PAC193x sensor module
* \param config             Pointer to the configuration struct
* \param enable_channels    Boolean for enabling all channels (if true, all channels are enabled, otherwise they are disabled)
* \return true if set successfully, false otherwise
*/
bool pac193x_enable_all_channels(pac193x_t *config, bool enable_channels);


/*! \brief Function for setting the signal polarity of the voltage measurement using PAC193x sensor module
* \param config             Pointer to the configuration struct
* \param use_bipolar        Boolean for setting the polarity for measurement the bus voltage (true = bipolar [-32 ... + 32 V], false = unipolar [0 ... 32 V])
* \return true if set successfully, false otherwise
*/
bool pac193x_polarity_voltage(pac193x_t *config, bool use_bipolar);


/*! \brief Function for setting the signal polarity of the current measurement using PAC193x sensor module
* \param config             Pointer to the configuration struct
* \param use_bipolar        Boolean for setting the polarity for measurement the current flow (true = bipolar [-100 ... + 100 mV], false = unipolar [0 ... 100 mV])
* \return true if set successfully, false otherwise
*/
bool pac193x_polarity_current(pac193x_t *config, bool use_bipoar);


/*! \brief Initialize the PAC193x sensor module
* \param config             Pointer to the configuration struct
* \return true if initialization was successful, false otherwise
*/
bool pac193x_init(pac193x_t *config);


/*! \brief Function for triggering single conversion event and updating data registers (also in continous process) using the PAC193x sensor module
* \param config             Pointer to the configuration struct
* \return true if initialization was successful, false otherwise
*/
bool pac193x_update_data_register(pac193x_t *config);


/*! \brief Reading the actual voltage sample at selected channel using PAC193x current sensor module
* \param config             Pointer to the configuration struct
* \param channel            Channel to read (0-3)
* \return uint16_t with voltage reading with scaling factor of 0.488 mV/LSB (unipolar = +32 V / 2 ** 16 bit, bipolar = +/- 32 V / 2 ** 15 bit)
*/
uint16_t pac193x_read_voltage(pac193x_t *config, uint8_t channel);


/*! \brief Reading the voltage sample (mean of rolling 8) at selected channel using PAC193x current sensor module
* \param config             Pointer to the configuration struct
* \param channel            Channel to read (0-3)
* \return uint16_t with voltage read-out with scaling factor of 0.488 mV/LSB (unipolar = +32 V / 2 ** 16 bit, bipolar = +/- 32 V / 2 ** 15 bit)
*/
uint16_t pac193x_read_voltage_rolling(pac193x_t *config, uint8_t channel);


/*! \brief Reading the actual current flow sample of selected channel using PAC193x current sensor module
* \param config             Pointer to the configuration struct
* \param channel            Channel to read (0-3)
* \return uint16_t with voltage read-out with scaling factor of 1.53 µV/(LSB * R_sh) (unipolar = +100 mV / 2 ** 16 bit, bipolar = +/- 100 mV / 2 ** 15 bit)
*/
uint16_t pac193x_read_current(pac193x_t *config, uint8_t channel);


/*! \brief Reading the current flow (mean of rolling 8) of selected channel using PAC193x current sensor module
* \param config             Pointer to the configuration struct
* \param channel            Channel to read (0-3)
* \return uint16_t with voltage read-out with scaling factor of 1.53 µV/(LSB * R_sh) (unipolar = +100 mV / 2 ** 16 bit, bipolar = +/- 100 mV / 2 ** 15 bit)
*/
uint16_t pac193x_read_current_rolling(pac193x_t *config, uint8_t channel);


/*! \brief Reading the actual power sample of selected channel using PAC193x current sensor module
* \param config             Pointer to the configuration struct
* \param channel            Channel to read (0-3)
* \return uint32_t with digital value from power read-out (need scaling)
*/
uint32_t pac193x_read_power(pac193x_t *config, uint8_t channel);


/*! \brief Reading the actual power consumption (accumulated) of selected channel using PAC193x current sensor module
* \param config             Pointer to the configuration struct
* \param channel            Channel to read (0-3)
* \return uint48_t with running/ accumulate digital value from power measurement (need scaling and divison with numbers)
*/
uint64_t pac193x_read_power_accumulated(pac193x_t *config, uint8_t channel);


/*! \brief Reading the accumulation number from conversion using PAC193x current sensor module
* \param config             Pointer to the configuration struct
* \return uint24_t with number of accumulation numbers between two refresh requests
*/
uint32_t pac193x_read_accumulation_number(pac193x_t *config);

/*! \brief Reads voltage values on all channels
* \param config             Pointer to the configuration struct
* \param out                Pointer to buffer array
* \param len                Size of buffer array
* \return                   True if read was successful
*/
bool pac193x_read_all_voltages(pac193x_t *config, uint16_t *out, size_t len);

/*! \brief Reads current values on all channels
* \param config             Pointer to the configuration struct
* \param out                Pointer to buffer array. Int16 because of bipolar mode
* \param len                Size of buffer array
* \return                   True if read was successful
*/
bool pac193x_read_all_currents(pac193x_t *config, int16_t *out, size_t len);

/*! \brief Reads power values on all channels
* \param config             Pointer to the configuration struct
* \param out                Pointer to buffer array
* \param len                Size of buffer array
* \return                   True if read was successful
*/
bool pac193x_read_all_power(pac193x_t *config, uint32_t *out, size_t len);

/*! \brief Reading accumulated power values on all channels
* \param config             Pointer to the configuration struct
* \param out                Pointer to buffer array
* \param len                Size of buffer array
* \return                   True if read was successful
*/
bool pac193x_read_all_power_accumulated(pac193x_t *config, uint64_t *out, size_t len);

#endif