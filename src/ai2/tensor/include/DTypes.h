#ifndef ELASTIC_AI_RUNTIME_ENV5_DTYPES_H
#define ELASTIC_AI_RUNTIME_ENV5_DTYPES_H

#include "Rounding.h"
#include <stddef.h>
#include <stdint.h>

typedef enum dtype
{
    INT32,
    FLOAT32,
    LINEAR
} dtype_t;

/*! @brief Describes linear quantization
 * @param scale:
 * @param zeroPoint:
 * @param qMin:
 * @param qMax:
 */

typedef struct linearQ
{
    float scale;
    int16_t zeroPoint;
    uint8_t qMax;
    roundingMode_t roundingMode;
} linearQ_t;


typedef struct quantization
{
    dtype_t type;
    void* qConfig;
} quantization_t;

quantization_t *initQuantization(dtype_t type);

int32_t readBytesAsInt32(uint8_t *bytes);
float readBytesAsFloat(uint8_t *bytes);

float findMaxFloat(uint8_t *bytes, size_t numberOfElements, size_t bytesPerElement);
float findMinFloat(uint8_t *bytes, size_t numberOfElements, size_t bytesPerElement);

int32_t findMaxInt32(uint8_t *bytes, size_t numberOfElements, size_t bytesPerElement);
int32_t findMinInt32(uint8_t *bytes, size_t numberOfElements, size_t bytesPerElement);

#endif // ELASTIC_AI_RUNTIME_ENV5_DTYPES_H
