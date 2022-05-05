//
// Created by David P. Federl
//

#ifndef ENV5_PAC193X_TYPEDEFS
#define ENV5_PAC193X_TYPEDEFS

#include <stdint.h>

typedef struct i2c_inst i2c_inst_t;

typedef struct {
    uint8_t     i2c_slave_address;
    i2c_inst_t *i2c_host;
    float       rSense[4];
    /* Channels 1 to maxChannelNumber are going to be used.
    Note that some channels might be disabled by the ctrl-register. */
    uint8_t highestChannelInUse;
  } pac193x_sensor_configuration;

typedef struct {
    uint8_t product_id;
    uint8_t manufacturer_id;
    uint8_t revision_id;
  } pac193x_info;

typedef struct {
    float voltageSource;
    float voltageSense;
    float iSense;
    float powerActual;
    float energy;
  } pac193x_measurements;

typedef struct {
    uint8_t command;
    float (*calculationFunction)(uint64_t value, uint8_t channel);
    uint8_t sizeOfResponseBuffer;
  } pac193x_measurement_properties;

enum {
    PAC193X_CMD_REFRESH              = 0x00,
    PAC193X_CMD_CTRL                 = 0x01,
    PAC193X_CMD_READ_ACC_COUNT       = 0x02,
    PAC193X_CMD_READ_VPOWER1_ACC     = 0x03,
    PAC193X_CMD_READ_VPOWER2_ACC     = 0x04,
    PAC193X_CMD_READ_VPOWER3_ACC     = 0x05,
    PAC193X_CMD_READ_VPOWER4_ACC     = 0x06,
    PAC193X_CMD_READ_VBUS1           = 0x07,
    PAC193X_CMD_READ_VBUS2           = 0x08,
    PAC193X_CMD_READ_VBUS3           = 0x09,
    PAC193X_CMD_READ_VBUS4           = 0x0A,
    PAC193X_CMD_READ_VSENSE1         = 0x0B,
    PAC193X_CMD_READ_VSENSE2         = 0x0C,
    PAC193X_CMD_READ_VSENSE3         = 0x0D,
    PAC193X_CMD_READ_VSENSE4         = 0x0E,
    PAC193X_CMD_READ_VBUS1_AVG       = 0x0F,
    PAC193X_CMD_READ_VBUS2_AVG       = 0x10,
    PAC193X_CMD_READ_VBUS3_AVG       = 0X11,
    PAC193X_CMD_READ_VBUS4_AVG       = 0X12,
    PAC193X_CMD_READ_VSENSE1_AVG     = 0X13,
    PAC193X_CMD_READ_VSENSE2_AVG     = 0X14,
    PAC193X_CMD_READ_VSENSE3_AVG     = 0X15,
    PAC193X_CMD_READ_VSENSE4_AVG     = 0X16,
    PAC193X_CMD_READ_VPOWER1         = 0X17,
    PAC193X_CMD_READ_VPOWER2         = 0X18,
    PAC193X_CMD_READ_VPOWER3         = 0X19,
    PAC193X_CMD_READ_VPOWER4         = 0X1A,
    PAC193X_CMD_CHANNEL_DIS          = 0X1C,
    PAC193X_CMD_NEG_PWR              = 0X1D,
    PAC193X_CMD_REFRESH_G            = 0x1E,
    PAC193X_CMD_REFRESH_V            = 0x1F,
    PAC193X_CMD_SLOW                 = 0X20,
    PAC193X_CMD_READ_CTRL_ACT        = 0X21,
    PAC193X_CMD_READ_CHANNEL_DIS_ACT = 0X22,
    PAC193X_CMD_READ_NEG_PWR_ACT     = 0X23,
    PAC193X_CMD_CTRL_LAT             = 0X24,
    PAC193X_CMD_CHANNEL_DIS_LAT      = 0X25,
    PAC193X_CMD_NEG_PWR_LAT          = 0x26,
    PAC193X_CMD_READ_PRODUCT_ID      = 0xFD,
    PAC193X_CMD_READ_MANUFACTURER_ID = 0xFE,
    PAC193X_CMD_READ_REVISION_ID     = 0xFF,
  };
typedef uint8_t pac193x_registerAddress;
typedef uint8_t pac193x_settings;

/*! \enum pac193x_Channels
 *
 *  Channels of pac193x
 *  The Sensors are connected to channel 1
 *  Wifi is connected to channel 2
 *  channels 3 and 4 are currently not used.
 */
enum {
    PAC193X_CHANNEL_SENSORS = 1,
    PAC193X_CHANNEL_WIFI    = 2,
  };
typedef uint8_t pac193x_channel;

/* values with postfix '_AVG' are the rolling
 * average of the last eight values.
 */
enum {
    PAC193X_VSOURCE,
    PAC193X_VSOURCE_AVG,
    PAC193X_VSENSE,
    PAC193X_VSENSE_AVG,
    PAC193X_ISENSE,
    PAC193X_ISENSE_AVG,
    PAC193X_PACTUAL,
    PAC193X_ENERGY,
  };
typedef uint8_t pac193x_valueToMeasure;

enum {
    PAC193X_NO_ERROR            = 0x00,
    PAC193X_SEND_COMMAND_ERROR  = 0x01,
    PAC193X_RECEIVE_DATA_ERROR  = 0x02,
    PAC193X_CHECKSUM_ERROR      = 0x03,
    PAC193X_INIT_ERROR          = 0x10,
    PAC193X_PARM_ERROR          = 0x11,
    PAC193X_INVALID_MEASUREMENT = 0x12,
    PAC193X_INVALID_CHANNEL     = 0x13,
    PAC193X_UNDEFINED_ERROR     = 0x20,
  };
typedef uint8_t pac193x_errorCode;

#endif /* ENV5_PAC193X_TYPEDEFS */

