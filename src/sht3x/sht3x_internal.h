//
// Created by David P. Federl
//

#ifndef ENV5_SHT3X_INTERNAL_H
#define ENV5_SHT3X_INTERNAL_H

#include "typedefs.h"
#include <stdint.h>

/****************************************************/
/* region CONSTANTS */

static const uint16_t CRC8_POLYNOMIAL = 0x31;             /* P(x) = x^8 + x^5 + x^4 + 1 = 00110001= 0x31 */
static const float    DENOMINATOR     = (1 << 16) - 1.0f; /* 2^16 - 1 */

/* endregion */
/****************************************************/
/* region FUNCTION PROTOTYPES */

static sht3x_errorCode sendRequestToSensor(sht3x_command command);

static sht3x_errorCode receiveDataFromSensor(uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer);

static sht3x_errorCode performChecksumCheck(const uint8_t *responseBuffer, uint8_t sizeOfResponseBuffer);

static uint8_t calculateChecksum(const uint8_t *dataBuffer);

static float calculateTemperature(uint16_t rawValue);

static float calculateHumidity(uint16_t rawValue);

/* endregion */

#endif // ENV5_SHT3X_INTERNAL_H
