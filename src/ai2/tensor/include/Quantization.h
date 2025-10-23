#ifndef ENV5_RUNTIME_QUANTIZATION_H
#define ENV5_RUNTIME_QUANTIZATION_H
#include "Rounding.h"

typedef enum qtype
{
    INT32,
    FLOAT32,
    LINEAR
} qtype_t;

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
    qtype_t type;
    void* qConfig;
} quantization_t;

void initQuantization(qtype_t type, quantization_t *quantization);

#endif // ENV5_RUNTIME_QUANTIZATION_H
