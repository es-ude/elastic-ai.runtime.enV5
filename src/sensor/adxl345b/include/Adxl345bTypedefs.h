#ifndef ENV5_ADXL345B_TYPEDEFS
#define ENV5_ADXL345B_TYPEDEFS

#include <stdint.h>

/* abstraction to avoid dependencies to pico libraries */
typedef struct i2c_inst i2c_inst_t;

/* Available register values for the ADXL345B_REGISTER_BW_RATE setting specific Hz rates for the
 * sensor.
 * LPM for Low Power Mode.
 */
enum {

    //!  represents LPM_BW_RATE 12.5
    ADXL345B_BW_RATE_LPM_12_point_5 = 0b00010111,
    ADXL345B_BW_RATE_LPM_25 = 0b00011000,
    ADXL345B_BW_RATE_LPM_50 = 0b00011001,
    ADXL345B_BW_RATE_LPM_100 = 0b00011010,
    ADXL345B_BW_RATE_LPM_200 = 0b00011011,
    ADXL345B_BW_RATE_LPM_400 = 0b00011100,

    //!  represents BW_RATE 0.1
    ADXL345B_BW_RATE_0_point_1 = 0b00010000,
    //!  represents BW_RATE 0.2
    ADXL345B_BW_RATE_0_point_2 = 0b00010001,
    ADXL345B_BW_RATE_0_point_39 = 0b00010010, //!<  represents BW_RATE0.39
    //!  represents BW_RATE 0.78
    ADXL345B_BW_RATE_0_point_78 = 0b00010011,
    //!  represents BW_RATE 0.56
    ADXL345B_BW_RATE_1_point_56 = 0b00010100,
    //!  represents BW_RATE 3.13
    ADXL345B_BW_RATE_3_point_13 = 0b00010101,
    //!  represents BW_RATE 6.25
    ADXL345B_BW_RATE_6_point_25 = 0b00010110,
    //!  represents BW_RATE 12.5
    ADXL345B_BW_RATE_12_point_5 = 0b00010111,
    ADXL345B_BW_RATE_25 = 0b00001000,
    ADXL345B_BW_RATE_50 = 0b00001001,
    ADXL345B_BW_RATE_100 = 0b00001010,
    ADXL345B_BW_RATE_200 = 0b00001011,
    ADXL345B_BW_RATE_400 = 0b00001100,
    ADXL345B_BW_RATE_800 = 0b00001101,

    // ADXL345B_BW_RATE_1600 = 0b00001110, //!< 1600 only available for SPI
    // ADXL345B_BW_RATE_3200 = 0b00001111, //!< 3200  only available for SPI
    
};

/* i2c address of the sensor
 *
 * defines the possible i2c addresses of the sensor:
 * ADXL345B_I2C_ALTERNATE_ADDRESS should be chosen if the ALT ADDRESS pin (12)
 * is connected to GND ADXL345B_I2C_ADDRESS otherwise
 */
typedef enum adxl345bI2cSlaveAddress {
    ADXL345B_I2C_ADDRESS = 0x1D,
    ADXL345B_I2C_ALTERNATE_ADDRESS = 0x53,
} adxl345bI2cSlaveAddress_t;

typedef struct adxl345bI2cConfiguration {
    adxl345bI2cSlaveAddress_t i2c_slave_address;
    i2c_inst_t *i2c_host;
} adxl345bI2cSensorConfiguration_t;

/* All Register + address of the ADXL345B */
typedef enum adxl345Register {
    ADXL345B_REGISTER_DEVICE_ID = 0x00,
    ADXL345B_TAP_THRESHOLD = 0x1D,
    ADXL345B_OFFSET_X = 0x1E,
    ADXL345B_OFFSET_Y = 0x1F,
    ADXL345B_OFFSET_Z = 0x20,
    ADXL345B_TAB_DURATION = 0x21,
    ADXL345B_TAB_LATENCY = 0x22,
    ADXL345B_TAB_WINDOW = 0x23,
    ADXl345B_ACTIVITY_THRESHOLD = 0x24,
    ADXL345B_INACTIVITY_THERSHOLD = 0x25,
    ADXL345B_INACTIVITY_TIME = 0x26,
    ADXL345B_AXIS_FOR_ACTIVITY_DETECTION = 0x27,
    ADXL345B_FREE_FALL_THERSHOLD = 0x28,
    ADXL345B_FREE_FALL_TIME = 0x29,
    ADXL345B_TAP_AXIS = 0x2A,
    ADXL345B_TAP_AXIS_SOURCE = 0x2B,
    ADXL345B_REGISTER_BW_RATE = 0x2C,
    ADXL345B_REGISTER_POWER_CONTROL = 0x2D,
    ADXL345B_REGISTER_INTERRUPT_ENABLE = 0x2E,
    ADXL345B_REGISTER_INTERRUPT_MAP = 0x2F,
    ADXL345B_REGISTER_INTERRUPT_SOURCE = 0x30,
    ADXL345B_REGISTER_DATA_FORMAT = 0x31,

    // Register 0x32 to 0x37 READ ONLY
    ADXL345B_REGISTER_DATA_X = 0x32,
    ADXL345B_REGISTER_DATA_Y = 0x34,
    ADXL345B_REGISTER_DATA_Z = 0x36,
    ADXL345B_FIFO_CONTROL = 0x38,
    ADXL345B_FIFO_STATUS = 0x39
} adxl345bRegister_t;

typedef uint8_t adxl345bConfiguration_t;


typedef enum adxl345bRange {
    ADXL345B_2G_RANGE,
    ADXL345B_4G_RANGE,
    ADXL345B_8G_RANGE,
    ADXL345B_16G_RANGE,
} adxl345bRange_t;

typedef enum adxl345bErrorCode {
    ADXL345B_NO_ERROR = 0x00,
    ADXL345B_SEND_COMMAND_ERROR = 0x01,
    ADXL345B_RECEIVE_DATA_ERROR = 0x02,
    ADXL345B_CHECKSUM_ERROR = 0x03,
    ADXL345B_INIT_ERROR = 0x10,
    ADXL345B_PARM_ERROR = 0x11,
    ADXL345B_CONFIGURATION_ERROR = 0x12,
    ADXL345B_UNDEFINED_ERROR = 0x20,
    ADXL345B_SELF_TEST_FAILED_FOR_X = 0x31,
    ADXL345B_SELF_TEST_FAILED_FOR_Y = 0x32,
    ADXL345B_SELF_TEST_FAILED_FOR_Z = 0x33,
} adxl345bErrorCode_t;

#endif /* ENV5_ADXL345B_TYPEDEFS */
