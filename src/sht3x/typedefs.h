#ifndef ENV5_SHT3X_TYPEDEFS
#define ENV5_SHT3X_TYPEDEFS

#include <stdint.h>

typedef struct i2c_inst i2c_inst_t;

typedef struct {
    uint8_t i2c_slave_address;
    i2c_inst_t *i2c_host;
} sht3x_i2c_sensorConfiguration;

enum {
    SHT3X_CMD_READ_SERIALNUMBER = 0x3780, /*!< read serial number */
    SHT3X_CMD_READ_STATUS = 0xF32D,       /*!< read status register */
    SHT3X_CMD_CLEAR_STATUS = 0x3041,      /*!< clear status register */
    SHT3X_CMD_HEATER_ENABLE = 0x306D,     /*!< enabled heater */
    SHT3X_CMD_HEATER_DISABLE = 0x3066,    /*!< disable heater */
    SHT3X_CMD_SOFT_RESET = 0x30A2,        /*!< soft reset */
    SHT3X_CMD_MEASURE_CLOCKSTRETCH_HIGH =
        0x2C06, /*!< measurement: clock stretching, high repeatability */
    SHT3X_CMD_MEASURE_CLOCKSTRETCH_MEDIUM =
        0x2C0D, /*!< measurement: clock stretching, medium repeatability */
    SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW =
        0x2C10, /*!< measurement: clock stretching, low repeatability */
    SHT3X_CMD_MEASURE_POLLING_HIGH =
        0x2400, /*!< measurement: polling, high repeatability */
    SHT3X_CMD_MEASURE_POLLING_MEDIUM =
        0x240B, /*!< measurement: polling, medium repeatability */
    SHT3X_CMD_MEASURE_POLLING_LOW =
        0x2416, /*!< measurement: polling, low repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_05_HIGH =
        0x2032, /*!< measurement: periodic 0.5 mps, high repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_05_MEDIUM =
        0x2024, /*!< measurement: periodic 0.5 mps, medium repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_05_LOW =
        0x202F, /*!< measurement: periodic 0.5 mps, low repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_1_HIGH =
        0x2130, /*!< measurement: periodic 1 mps, high repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_1_MEDIUM =
        0x2126, /*!< measurement: periodic 1 mps, medium repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_1_LOW =
        0x212D, /*!< measurement: periodic 1 mps, low repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_2_HIGH =
        0x2236, /*!< measurement: periodic 2 mps, high repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_2_MEDIUM =
        0x2220, /*!< measurement: periodic 2 mps, medium repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_2_LOW =
        0x222B, /*!< measurement: periodic 2 mps, low repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_4_HIGH =
        0x2334, /*!< measurement: periodic 4 mps, high repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_4_MEDIUM =
        0x2322, /*!< measurement: periodic 4 mps, medium repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_4_LOW =
        0x2329, /*!< measurement: periodic 4 mps, low repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_10_HIGH =
        0x2737, /*!< measurement: periodic 10 mps, high repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_10_MEDIUM =
        0x2721, /*!< measurement: periodic 10 mps, medium repeatability */
    SHT3X_CMD_MEASURE_PERIODIC_10_LOW =
        0x272A, /*!< measurement: periodic 10 mps, low repeatability */
    SHT3X_CMD_FETCH_DATA =
        0xE000, /*!< readout measurements for periodic mode */
    SHT3X_CMD_READ_ALERT_LIMITS_LOW = 0xE102, /*!< read alert limits, low set */
    SHT3X_CMD_READ_ALERT_LIMITS_LOW_CLEAR =
        0xE109, /*!< read alert limits, low clear */
    SHT3X_CMD_READ_ALERT_LIMITS_HIGH_SET =
        0xE11F, /*!< read alert limits, high set */
    SHT3X_CMD_READ_ALERT_LIMITS_HIGH_CLEAR =
        0xE114, /*!< read alert limits, high clear */
    SHT3X_CMD_WRITE_ALERT_LIMITS_HIGH_SET =
        0x611D, /*!< write alert limits, high set */
    SHT3X_CMD_WRITE_ALERT_LIMITS_HIGH_CLEAR =
        0x6116, /*!< write alert limits, high clear */
    SHT3X_CMD_WRITE_ALERT_LIMITS_LOW_CLEAR =
        0x610B, /*!< write alert limits, low clear */
    SHT3X_CMD_WRITE_ALERT_LIMITS_LOW_SET =
        0x6100,                  /*!< write alert limits, low set */
    SHT3X_CMD_NO_SLEEP = 0x303E, /*!< disable sleep mode */
};
typedef uint16_t sht3x_command;

enum {
    SHT3X_NO_ERROR = 0x00,
    SHT3X_SEND_COMMAND_ERROR = 0x01,
    SHT3X_RECEIVE_DATA_ERROR = 0x02,
    SHT3X_CHECKSUM_ERROR = 0x03,
    SHT3X_INIT_ERROR = 0x10,
    SHT3X_PARM_ERROR = 0x11,
    SHT3X_UNDEFINED_ERROR = 0x20,
};
typedef uint8_t sht3x_errorCode;

/*! \union SHT3X_StatusRegister
 *  \struct bits
 *
 * Defines the structure of the status register for a LITTLE_ENDIAN based
 * machine
 */
typedef union {
    uint16_t config; /*!< register as one 16 bit field */
    struct {
        uint16_t crcStatus : 1;     /*!< write data checksum status */
        uint16_t cmdStatus : 1;     /*!< command status */
        uint16_t reserve0 : 2;      /*!< reserved */
        uint16_t resetDetected : 1; /*!< system reset detected */
        uint16_t reserve1 : 5;      /*!< reserved */
        uint16_t tAlert : 1;        /*!< temperature tracking alert */
        uint16_t rhAlert : 1;       /*!< humidity tracking alert */
        uint16_t reserve2 : 1;      /*!< reserved */
        uint16_t heaterStatus : 1;  /*!< heater status */
        uint16_t reserve3 : 1;      /*!< reserved */
        uint16_t alertPending : 1;  /*!< alert pending status */
    } bits;
} sht3x_statusRegister;

#endif /* ENV5_SHT3X_TYPEDEFS */
