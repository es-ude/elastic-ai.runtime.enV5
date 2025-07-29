#ifndef ENV5_BMI323_INTERFACE_HEADER
#define ENV5_BMI323_INTERFACE_HEADER

#include <stdbool.h>
#include <stdint.h>

#include "bmi323_defs.h"
#include "bmi3_defs.h"

#include "eai/hal/SpiTypedefs.h"

/**
 * @defgroup bmi323
 * @brief BMI323 Sensor API
 */

/* region TYPEDEFS */
/**
 * @ingroup bmi323
 * @defgroup bmi323Types Typedefs
 * @brief Typedefs for the BMI323 Library
 */

/*!
 * @brief possible error codes for the API
 * @ingroup bmi323Types
 */
typedef enum bmi323ErrorCodes {
    BMI323_ERROR_NULL_PTR = BMI3_E_NULL_PTR,
    BMI323_ERROR_COM_FAIL = BMI3_E_COM_FAIL,
    BMI323_ERROR_DEV_NOT_FOUND = BMI3_E_DEV_NOT_FOUND,
    BMI323_ERROR_ACC_INVALID_CFG = BMI3_E_ACC_INVALID_CFG,
    BMI323_ERROR_GYRO_INVALID_CFG = BMI3_E_GYRO_INVALID_CFG,
    BMI323_ERROR_INVALID_SENSOR = BMI3_E_INVALID_SENSOR,
    BMI323_ERROR_INVALID_INT_PIN = BMI3_E_INVALID_INT_PIN,
    BMI323_ERROR_INVALID_INPUT = BMI3_E_INVALID_INPUT,
    BMI323_ERROR_INVALID_STATUS = BMI3_E_INVALID_STATUS,
    BMI323_ERROR_DATA_RDY_INT_FAILED = BMI3_E_DATA_RDY_INT_FAILED,
    BMI323_ERROR_INVALID_FOC_POSITION = BMI3_E_INVALID_FOC_POSITION,
    BMI323_ERROR_INVALID_ST_SELECTION = BMI3_E_INVALID_ST_SELECTION,
    BMI323_ERROR_OUT_OF_RANGE = BMI3_E_OUT_OF_RANGE,
    BMI323_ERROR_FEATURE_ENGINE_STATUS = BMI3_E_FEATURE_ENGINE_STATUS,
} bmi323ErrorCodes_t;

/*!
 * @brief struct holding the configuration for the sensor
 * @ingroup bmi323Types
 */
typedef struct bmi3_dev bmi323SensorConfiguration_t;

/*!
 * @brief struct holding the interrupt configuration
 * @ingroup bmi323Types
 */
typedef struct bmi3_int_pin_config bmi323InterruptConfig_t;

/*!
 * @brief struct holding mapping for a specific interrupt
 * @ingroup bmi323Types
 */
typedef struct bmi3_map_int bmi323InterruptMapping_t;

/*!
 * @brief struct holding the configuration for an interrupt register
 * @ingroup bmi323Types
 */
typedef enum bmi323InterruptRegister {
    BMI323_INTERRUPT_1 = BMI3_INT1,
    BMI323_INTERRUPT_2 = BMI3_INT2,
} bmi323InterruptRegister_t;

/*!
 * @brief struct holding the remapped axes Information
 * @ingroup bmi323Types
 */
typedef struct bmi3_axes_remap bmi323AxesRemap_t;

/*!
 * @brief struct representing the feature-enable-register
 * @ingroup bmi323Types
 */
typedef struct bmi3_feature_enable bmi323Features_t;

/*!
 * @brief struct representing the configuration for a special feature
 * @ingroup bmi323Types
 */
typedef struct bmi3_sens_config bmi323FeatureConfiguration_t;

/*!
 * @brief test results of self-test
 * @ingroup bmi323Types
 */
typedef struct bmi3_st_result bmi323TestResults_t;

/*!
 * @brief result of selb calibration run
 * @ingroup bmi323Types
 */
typedef struct bmi3_self_calib_rslt bmi323SelfCalibrationResults_t;

/*!
 * @brief struct holding sensor read from the sensor
 * @ingroup bmi323Types
 */
typedef struct bmi3_sensor_data bmi323SensorData_t;

/*!
 * @brief struct holding the data of a read FIFO frame
 * @ingroup bmi323Types
 */
typedef struct bmi3_fifo_frame bmi323FifoFrame_t;

/*!
 * @brief struct to hold extracted accelerator/gyroscope data from FIFO frame
 * @ingroup bmi323Types
 */
typedef struct bmi3_fifo_sens_axes_data bmi323AxesData_t;

/*!
 * @brief struct to hold extracted temperature data from FIFO frame
 * @ingroup bmi323Types
 */
typedef struct bmi3_fifo_temperature_data bmi323TemperatureData_t;

/* endregion TYPEDEFS */

/* region PROTOTYPES */

/* region CONFIG */
// tag::prototypes_config[]
/**
 * @ingroup bmi323
 * @defgroup bmi323APIConfig API for the general sensor configuration
 * @brief Functions to initialize/Configure the sensor in general
 *
 * @important We highly recommend using the "enV5_hw_configuration"-library
 */

/*!
 * @brief initializes the sensor
 * @ingroup bmi323APIConfig
 *
 * @code
 * void bmi323Init (
 *      bmi323SensorConfiguration_t *sensor
 * );
 * @endcode
 *
 * @param[in,out] sensor struct holding the sensor configuration
 * @param[in] spi spi configuration
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323Init(bmi323SensorConfiguration_t *sensor, spiConfiguration_t *spi);

/*!
 * @brief get data from register
 * @ingroup bmi323GeneralAPI
 *
 * @code
 * void bmi323GetRegister (
 *      bmi323SensorConfiguration_t *sensor,
 *      uint8_t address,
 *      data_t *data
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] address address of the register to read
 * @param[out] data buffer for data read from sensor
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323GetRegister(bmi323SensorConfiguration_t *sensor, uint8_t address, data_t *data);

/*!
 * @brief set register
 * @ingroup bmi323APIConfig
 *
 * @code
 * void bmi323SetRegister (
 *      bmi323SensorConfiguration_t *sensor,
 *      uint8_t address,
 *      data_t *data
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] address register to overwrite
 * @param[in] data data to write into register
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323SetRegister(bmi323SensorConfiguration_t *sensor, uint8_t address, data_t *data);

/*!
 * @brief enable/disable different sensor features
 * @ingroup bmi323APIConfig
 *
 * @code
 * void bmi323EnableFeature (
 *      bmi323SensorConfiguration_t *sensor,
 *      bmi323Features *features
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] features Features to be enabled
 *
 * @Features
 * @verbatim
 * Features                 |  Value
 * -------------------------|-----------
 * BMI323_ACCEL             |      0
 * BMI323_GYRO              |      1
 * BMI323_SIG_MOTION        |      2
 * BMI323_ANY_MOTION        |      3
 * BMI323_NO_MOTION         |      4
 * BMI323_STEP_DETECTOR     |      5
 * BMI323_STEP_COUNTER      |      6
 * BMI323_TILT              |      7
 * BMI323_ORIENTATION       |      8
 * BMI323_FLAT              |      9
 * BMI323_WAKEUP            |     10
 * BMI323_I3C_SYC           |     11
 * BMI323_GYRO_GAIN_UPDATE  |     12
 * BMI323_TEMP              |     16
 * BMI323_GYRO_SELF_OFF     |     18
 * @endverbatim
 *
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323EnableFeature(bmi323SensorConfiguration_t *sensor, bmi323Features_t *features);

/*!
 * @brief configure different sensor features
 * @ingroup bmi323APIConfig
 *
 * @code
 * void bmi323SetSensorConfiguration (
 *      bmi323SensorConfiguration_t *sensor,
 *      uint8_t numberOfFeatures,
 *      bmi323FeatureConfiguration *config
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] numberOfFeatures number of sensor features to configure
 * @param[in] config array containing one or more feature configurations
 *
 * @Features
 * @verbatim
 * Features                 |  Value
 * -------------------------|-----------
 * BMI323_ACCEL             |      0
 * BMI323_GYRO              |      1
 * BMI323_SIG_MOTION        |      2
 * BMI323_ANY_MOTION        |      3
 * BMI323_NO_MOTION         |      4
 * BMI323_STEP_DETECTOR     |      5
 * BMI323_STEP_COUNTER      |      6
 * BMI323_TILT              |      7
 * BMI323_ORIENTATION       |      8
 * BMI323_FLAT              |      9
 * BMI323_WAKEUP            |     10
 * BMI323_I3C_SYC           |     11
 * BMI323_GYRO_GAIN_UPDATE  |     12
 * BMI323_TEMP              |     16
 * BMI323_GYRO_SELF_OFF     |     18
 * @endverbatim
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323SetSensorConfiguration(bmi323SensorConfiguration_t *sensor, uint8_t numberOfFeatures,
                                  bmi323FeatureConfiguration_t *config);

/*!
 * @brief get configuration of different sensor features
 * @ingroup bmi323APIConfig
 *
 * @code
 * void bmi323GetSensorConfiguration(
 *      bmi323SensorConfiguration_t *sensor,
 *      uint8_t numberOfFeatures,
 *      bmi323FeatureConfiguration *config
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] numberOfFeatures number of sensor features to read configuration from
 * @param[out] config  array containing one or more feature configurations
 *
 * @Features
 * @verbatim
 * Features                 |  Value
 * -------------------------|-----------
 * BMI323_ACCEL             |      0
 * BMI323_GYRO              |      1
 * BMI323_SIG_MOTION        |      2
 * BMI323_ANY_MOTION        |      3
 * BMI323_NO_MOTION         |      4
 * BMI323_STEP_DETECTOR     |      5
 * BMI323_STEP_COUNTER      |      6
 * BMI323_TILT              |      7
 * BMI323_ORIENTATION       |      8
 * BMI323_FLAT              |      9
 * BMI323_WAKEUP            |     10
 * BMI323_I3C_SYC           |     11
 * BMI323_GYRO_GAIN_UPDATE  |     12
 * BMI323_TEMP              |     16
 * BMI323_GYRO_SELF_OFF     |     18
 * @endverbatim
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323GetSensorConfiguration(bmi323SensorConfiguration_t *sensor, uint8_t numberOfFeatures,
                                  bmi323FeatureConfiguration_t *config);

/*!
 * @brief get interrupt configuration
 * @ingroup bmi323APIConfig
 *
 * @code
 * void bmi323GetInterruptConfig(
 *      bmi323SensorConfiguration_t *sensor,
 *      bmi323InterruptConfig_t *config
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[out] config struct holding the interrupt configuration
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323GetInterruptConfig(bmi323SensorConfiguration_t *sensor, bmi323InterruptConfig_t *config);

/*!
 * @brief set the interrupt configuration
 * @ingroup bmi323APIConfig
 *
 * @code
 * void bmi323SetInterruptConfig(
 *      bmi323SensorConfiguration_t *sensor,
 *      bmi323InterruptConfig_t *config
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] config struct holding the interrupt configuration
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323SetInterruptConfig(bmi323SensorConfiguration_t *sensor, bmi323InterruptConfig_t *config);

/*!
 * @brief map interrupt to pin
 * @ingroup bmi323APIConfig
 *
 * @code
 * void bmi323SetInterruptMapping(
 *      bmi323SensorConfiguration_t *sensor,
 *      bmi323InterruptMapping mapping
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] mapping mapping of interrupt and pin
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323SetInterruptMapping(bmi323SensorConfiguration_t *sensor,
                               bmi323InterruptMapping_t mapping);

/*!
 * @brief set the interrupt configuration of the sensor
 * @ingroup bmi323APIConfig
 *
 * @code
 * uint16_t bmi323GetInterruptStatus (
 *          bmi323SensorConfiguration_t *sensor,
 *          bmi323InterruptRegister_t interruptRegister
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] interruptRegister interrupt register to read
 *
 * @returns requested interrupt register
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
uint16_t bmi323GetInterruptStatus(bmi323SensorConfiguration_t *sensor,
                                  bmi323InterruptRegister_t interruptRegister);

/*!
 * @brief get remapping of axes
 * @ingroup bmi323APIConfig
 *
 * @code
 * void bmi323GetRemappingOfAxes(
 *      bmi323SensorConfiguration_t *sensor,
 *      bmi323AxesRemap *remapping
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[out] remapping buffer for axes remapping
 *
 * @returns requested interrupt register
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323GetRemappingOfAxes(bmi323SensorConfiguration_t *sensor, bmi323AxesRemap_t *remapping);

/*!
 * @brief remap axes to represent actual sensor application
 * @ingroup bmi323APIConfig
 *
 * @code
 * void bmi323SetRemappingOfAxes(
 *      bmi323SensorConfiguration_t *sensor,
 *      bmi323AxesRemap remapping
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] remapping struct providing the remapping
 *
 * @returns requested interrupt register
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323SetRemappingOfAxes(bmi323SensorConfiguration_t *sensor, bmi323AxesRemap_t remapping);
// end::prototypes_config[]
/* endregion CONFIG */

/* region DATA */
// tag::prototypes_data[]
/**
 * @ingroup bmi323
 * @defgroup bmi323APIData
 * @brief Functions to get measurements
 */

/*!
 * @brief read data from the sensor
 * @ingroup bmi323APIData
 *
 * @code
 * void bmi323GetData(
 *      bmi323SensorConfiguration_t *sensor,
 *      uint8_t numberOfFeatures,
 *      bmi323SensorData_t *data
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] numberOfFeatures number of sensor features to read from
 * @param[out]data array holding the buffer to store the read data
 *
 * @Features
 * @verbatim
 * Features                 |  Value
 * -------------------------|-----------
 * BMI323_ACCEL             |      0
 * BMI323_GYRO              |      1
 * BMI323_SIG_MOTION        |      2
 * BMI323_ANY_MOTION        |      3
 * BMI323_NO_MOTION         |      4
 * BMI323_STEP_DETECTOR     |      5
 * BMI323_STEP_COUNTER      |      6
 * BMI323_TILT              |      7
 * BMI323_ORIENTATION       |      8
 * BMI323_FLAT              |      9
 * BMI323_WAKEUP            |     10
 * BMI323_I3C_SYC           |     11
 * BMI323_GYRO_GAIN_UPDATE  |     12
 * BMI323_TEMP              |     16
 * BMI323_GYRO_SELF_OFF     |     18
 * @endverbatim
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323GetData(bmi323SensorConfiguration_t *sensor, uint8_t numberOfFeatures,
                   bmi323SensorData_t *data);

/*!
 * @brief read temperature from the sensor
 * @ingroup bmi323APIData
 *
 * @code
 * float bmi323GetTemperature(
 *          bmi323SensorConfiguration_t * sensor
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 *
 * @returns Temperature in degree Celsius
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
float bmi323GetTemperature(bmi323SensorConfiguration_t *sensor);

/*!
 * @brief read the current time from the sensor
 * @ingroup bmi323APIData
 *
 * @code
 * float bmi323GetSensorTime(
 *          bmi323SensorConfiguration_t * sensor
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 *
 * @returns time in seconds since power up
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
float bmi323GetSensorTime(bmi323SensorConfiguration_t *sensor);

/*!
 * @brief convert raw value to degree per second (DPS)
 * @ingroup bmi323APIData
 *
 * @code
 * float bmi323LsbToDps(
 *       int16_t rawValue,
 *       uint8_t range,
 *       uint8_t resolution
 * );
 * @endcode
 *
 * @param[in] rawValue raw value to be converted
 * @param[in] range dps range for value conversion
 * @param[in] resolution bit width of the raw value
 *
 * @returns value in degree per second
 */
float bmi323LsbToDps(int16_t rawValue, uint8_t range, uint8_t resolution);

/*!
 * @brief convert raw value to meter per second squared (MPS2)
 * @ingroup bmi323APIData
 *
 * @code
 * float bmi323LsbToMps2(
 *       int16_t rawValue,
 *       uint8_t range,
 *       uint8_t resolution
 * );
 * @endcode
 *
 * @param[in] rawValue raw value to be converted
 * @param[in] range mps2 range for value conversion
 * @param[in] resolution bit width of the raw value
 *
 * @returns value in meter per second squared
 */
float bmi323LsbToMps2(int16_t rawValue, uint8_t range, uint8_t resolution);
// end::prototypes_data[]
/* endregion DATA */

/* region FIFO */
// tag::prototypes_fifo[]
/**
 * @ingroup bmi323
 * @defgroup bmi323APIFifo
 * @brief functions to manage/access the internal FIFO buffer
 */

/*!
 * @brief setup/enable/disable FIFO buffer
 * @ingroup bmi323APIData
 *
 * @code
 * void bmi323SetFifoConfiguration(
 *      bmi323SensorConfiguration_t *sensor, bool enable,
 *      uint16_t configuration
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param enable[in] bool to disable/enable FIFO buffer
 * @param configuration[in] fifo configuration
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323SetFifoConfiguration(bmi323SensorConfiguration_t *sensor, bool enable,
                                uint16_t configuration);

/*!
 * @brief get FIFO buffer configuration
 * @ingroup bmi323APIData
 *
 * @code
 * uint16_t bmi323GetFifoConfiguration(
 *          bmi323SensorConfiguration_t *sensor
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 *
 * @returns FIFO buffer configuration
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
uint16_t bmi323GetFifoConfiguration(bmi323SensorConfiguration_t *sensor);

/*!
 * @brief get FIFO buffer length
 * @ingroup bmi323APIData
 *
 * @code
 * uint16_t bmi323GetFifoLength(
 *          bmi323SensorConfiguration_t *sensor
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 *
 * @returns FIFO buffer length
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
uint16_t bmi323GetFifoLength(bmi323SensorConfiguration_t *sensor);

/*!
 * @brief get FIFO buffer frame
 * @ingroup bmi323APIData
 *
 * @code
 * uint16_t bmi323GetFifoFrame(
 *          bmi323SensorConfiguration_t *sensor,
 *          bmi323FifoFrame_t *frame
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in,out] frame buffer/configuration for the FIFO frame to read
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323GetFifoFrame(bmi323SensorConfiguration_t *sensor, bmi323FifoFrame_t *frame);

/*!
 * @brief retrieve accelerometer data from FIFO frame
 * @ingroup bmi323APIData
 *
 * @code
 * void bmi323ExtractAcceleratorData(
 *      bmi323SensorConfiguration_t *sensor,
 *      bmi323FifoFrame_t *frame,
 *      bmi323AxesData_t *data
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] frame buffer/configuration for the FIFO frame to read
 * @param[in,out] data accelerometer data retrieved from @p frame
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323ExtractAcceleratorData(bmi323SensorConfiguration_t *sensor, bmi323FifoFrame_t *frame,
                                  bmi323AxesData_t *data);

/*!
 * @brief retrieve gyroscope data from FIFO frame
 * @ingroup bmi323APIData
 *
 * @code
 * void bmi323ExtractGyroscopeData(
 *      bmi323SensorConfiguration_t *sensor,
 *      bmi323FifoFrame_t *frame,
 *      bmi323AxesData_t *data
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] frame buffer/configuration for the FIFO frame to read
 * @param[in,out] data gyroscope data retrieved from @p frame
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323ExtractGyroscopeData(bmi323SensorConfiguration_t *sensor, bmi323FifoFrame_t *frame,
                                bmi323AxesData_t *data);

/*!
 * @brief retrieve temperature data from FIFO frame
 * @ingroup bmi323APIData
 *
 * @code
 * void bmi323ExtractTemperatureData(
 *      bmi323SensorConfiguration_t *sensor,
 *      bmi323FifoFrame_t *frame,
 *      bmi323TemperatureData_t *data
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] frame buffer/configuration for the FIFO frame to read
 * @param[in,out] data temperature data retrieved from @p frame
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323ExtractTemperatureData(bmi323SensorConfiguration_t *sensor, bmi323FifoFrame_t *frame,
                                  bmi323TemperatureData_t *data);
// end::prototypes_fifo[]
/* endregion FIFO */

/* region COMMANDS */
// tag::prototypes_config[]

/**
 * @ingroup bmi323
 * @defgroup bmi323APICommands
 * @brief functions to simplify access to dedicated functions
 */

/*!
 * @brief send command to the sensor
 * @ingroup bmi323APICommand
 *
 * @code
 * void bmi323SendCommand(
 *      bmi323SensorConfiguration_t *sensor,
 *      uint16_t command
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] command command to send to the sensor
 *
 * @verbatim
 * Commands                       |  Values
 * -------------------------------|---------------------
 * BMI3_CMD_SELF_TEST_TRIGGER     |  0x0100
 * BMI3_CMD_SELF_CALIB_TRIGGER    |  0x0101
 * BMI3_CMD_SELF_CALIB_ABORT      |  0x0200
 * BMI3_CMD_I3C_TCSYNC_UPDATE     |  0x0201
 * BMI3_CMD_AXIS_MAP_UPDATE       |  0x300
 * BMI3_CMD_USR_GAIN_OFFS_UPDATE  |  0x301
 * BMI3_CMD_1                     |  0x64AD
 * BMI3_CMD_2                     |  0xD3AC
 * BMI3_CMD_SOFT_RESET            |  0xDEAF
 * @endverbatim
 *
 * @throws ErrorCodes See #bmi323ErroCodes
 */
void bmi323SendCommand(bmi323SensorConfiguration_t *sensor, uint16_t command);

/*!
 * @brief perform soft reset of sensor
 * @ingroup bmi323APICommands
 *
 * @code
 * void bmi323SoftReset (
 *      bmi323SensorConfiguration_t *sensor
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323SoftReset(bmi323SensorConfiguration_t *sensor);

/*!
 * @brief perform self test
 * @ingroup bmi323APICommands
 *
 * @code
 * void bmi323PerformSelfTest(
 *      bmi323SensorConfiguration_t *sensor,
 *      uint8_t features,
 *      bmi323TestResults
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] features sensor features to test
 * @param[out] results results of the performed tests
 *
 * @verbatim
 * feature              |  Value
 * -------------------- |--------
 * BMI3_ST_ACCEL_ONLY   |      1
 * BMI3_ST_GYRO_ONLY    |      2
 * BMI3_ST_BOTH_ACC_GYR |      3
 * @endverbatim
 *
 * @throws ErrorCodes See #bmi323ErrorCodes
 */
void bmi323PerformSelfTest(bmi323SensorConfiguration_t *sensor, uint8_t features,
                           bmi323TestResults_t *results);

/*!
 * @brief perform self calibration of gyroscope
 * @ingroup bmi323APICommands
 *
 * @code
 * void bmi323PerformGyroscopeSelfCalibration(
 *      bmi323SensorConfiguration_t *sensor,
 *      uint8_t selfCalibrationFeature,
 *      uint8_t applyCorrection,
 *      bmi323SelfCalibrationResults* results
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 * @param[in] selfCalibrationFeature feature to use for self-calibration
 * @param[in] applyCorrection apply corrections after self-calibration
 * @param[out] results struct holding the results of the self-calibration
 *
 *
 * @verbatim
 * Self Calibration Feature | Value
 * -------------------------|------
 * BMI3_SC_SENSITIVITY_EN   | 1
 * BMI3_SC_OFFSET_EN        | 2
 * @endverbatim
 *
 * @verbatim
 * Apply Correction       | Value
 * -----------------------|------
 * BMI2_SC_APPLY_CORR_DIS | 0
 * BMI3_SC_APPLY_CORR_EN  | 4
 * @endverbatim
 *
 * @throws ErrorCodes see #bmi323ErrorCodes
 */
void bmi323PerformGyroscopeSelfCalibration(bmi323SensorConfiguration_t *sensor,
                                           uint8_t selfCalibrationFeature, uint8_t applyCorrection,
                                           bmi323SelfCalibrationResults_t *results);

/*!
 * @brief get the status from the sensor
 * @ingroup bmi323APICommands
 *
 * @code
 * void bmi323GetSensorStatus(
 *      bmi323SensorConfiguration_t *sensor,
 * );
 * @endcode
 *
 * @param[in] sensor struct holding the sensor configuration
 *
 * @returns sensor status
 *
 * @verbatim
 * Value    |  Status
 * ---------|---------------------
 * 0x01     |  BMI3_STATUS_POR
 * 0x20     |  BMI3_STATUS_DRDY_TEMP
 * 0x40     |  BMI3_STATUS_DRDY_GYR
 * 0x80     |  BMI3_STATUS_DRDY_ACC
 * @endverbatim
 *
 * @throws ErrorCodes see #bmi323ErrorCodes
 */
uint16_t bmi323GetSensorStatus(bmi323SensorConfiguration_t *sensor);
// end::prototypes_config[]
/* endregion COMMANDS */

/* endregion PROTOTYPES */

#endif // ENV5_BMI323_INTERFACE_HEADER
