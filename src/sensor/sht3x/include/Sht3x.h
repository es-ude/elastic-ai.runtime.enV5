#ifndef ENV5_SHT3X_HEADER
#define ENV5_SHT3X_HEADER

#include <stdint.h>

#include "Sht3xTypedefs.h"

// tag::prototypes[]
/*!
 * @brief initializes the temperature sensor
 *
 * @important function has to be called before use of the sensor can be used \n
 *            needs max 1.5ms for idle state after power up
 *
 * @important We highly recommend using the "enV5_hw_configuration" -library
 *
 * @param[in] sensor struct that holds parameters for communication with sensor
 *
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 * @throws SHT3X_SEND_COMMAND_ERROR if command can't be sent
 */
void sht3xInit(sht3xSensorConfiguration_t sensor);

/*!
 * @brief read the value of the serial number from the sensor
 *
 * @param[in] sensor struct that holds parameters for communication with sensor
 *
 * @returns serialNumber
 *
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 * @throws SHT3X_SEND_COMMAND_ERROR if request can't be sent
 * @throws SHT3X_RECEIVE_DATA_ERROR if receiving data failed
 * @throws SHT3X_CHECKSUM_ERROR if checksum of received doesn't match
 */
uint32_t sht3xReadSerialNumber(sht3xSensorConfiguration_t sensor);

/*!
 * @brief read the status register (settings) from the sensor
 *
 * @param[in]  sensor struct that holds parameters for communication with sensor
 * @param[out] statusRegister union that holds the actual values of the status register
 *
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 * @throws SHT3X_SEND_COMMAND_ERROR if request can't be sent
 * @throws SHT3X_RECEIVE_DATA_ERROR if receiving data failed
 * @throws SHT3X_CHECKSUM_ERROR if checksum of received doesn't match
 */
void sht3xReadStatusRegister(sht3xSensorConfiguration_t sensor,
                             sht3xStatusRegister_t statusRegister);

/*!
 * @brief read \b only the temperature from the sensor
 *
 * @param[in]  sensor struct that holds parameters for communication with sensor
 *
 * @returns temperature in degree celsius
 *
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 * @throws SHT3X_SEND_COMMAND_ERROR if request can't be sent
 * @throws SHT3X_RECEIVE_DATA_ERROR if receiving data failed
 * @throws SHT3X_CHECKSUM_ERROR if checksum of received doesn't match
 */
float sht3xGetTemperature(sht3xSensorConfiguration_t sensor);

/*!
 * @brief read \b only the humidity from the sensor
 *
 * @param[in]  sensor struct that holds parameters for communication with sensor
 *
 * @returns temperature in degree celsius
 *
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 * @throws SHT3X_SEND_COMMAND_ERROR if request can't be sent
 * @throws SHT3X_RECEIVE_DATA_ERROR if receiving data failed
 * @throws SHT3X_CHECKSUM_ERROR if checksum of received doesn't match
 */
float sht3xGetHumidity(sht3xSensorConfiguration_t sensor);

/*!
 * @brief read the temperature \b and the humidity from the sensor
 *
 * @param[in]  sensor struct that holds parameters for communication with sensor
 * @param[out] temperature temperature in degree celsius
 * @param[out] humidity relative humidity in percent
 *
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 * @throws SHT3X_SEND_COMMAND_ERROR if request can't be sent
 * @throws SHT3X_RECEIVE_DATA_ERROR if receiving data failed
 * @throws SHT3X_CHECKSUM_ERROR if checksum of received doesn't match
 */
void sht3xGetTemperatureAndHumidity(sht3xSensorConfiguration_t sensor, float *temperature,
                                    float *humidity);

/*!
 * @brief get the last measured value from the sensor buffer
 *
 * @param[in]  sensor struct that holds parameters for communication with sensor
 * @param[out] temperature temperature in degree celsius
 * @param[out] humidity relative humidity in percent
 *
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 * @throws SHT3X_SEND_COMMAND_ERROR if request can't be sent
 * @throws SHT3X_RECEIVE_DATA_ERROR if receiving data failed
 * @throws SHT3X_CHECKSUM_ERROR if checksum of received doesn't match
 */
void sht3xReadMeasurementBuffer(sht3xSensorConfiguration_t sensor, float *temperature,
                                float *humidity);

/*!
 * @brief enable the heater module of the sensor
 *
 * the heater can be used to check the plausibility of the measured values \n
 * the heater is automatically disabled after a reset
 *
 * @param[in]  sensor struct that holds parameters for communication with sensor
 *
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 * @throws SHT3X_SEND_COMMAND_ERROR if request can't be sent
 */
void sht3xEnableHeater(sht3xSensorConfiguration_t sensor);

/*!
 * @brief manually disable the heater module of the sensor
 *
 * @param[in]  sensor struct that holds parameters for communication with sensor
 *
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 * @throws SHT3X_SEND_COMMAND_ERROR if request can't be sent
 */
void sht3xDisableHeater(sht3xSensorConfiguration_t sensor);

/*!
 * @brief trigger a soft reset of the sensor which recalibrates the sensor and resets the system
 * controller
 *
 * @important - Hard RESET can be triggered by turning the power off and on again
 *
 * @param[in]  sensor struct that holds parameters for communication with sensor
 *
 * @throws SHT3X_INIT_ERROR if i2c not initialized
 * @throws SHT3X_SEND_COMMAND_ERROR if request can't be sent
 */
void sht3xSoftReset(sht3xSensorConfiguration_t sensor);
// end::prototypes[]

#endif /* ENV5_SHT3X_HEADER */
