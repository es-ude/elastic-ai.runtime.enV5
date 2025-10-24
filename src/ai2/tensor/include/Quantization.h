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
} linearQConfig_t;

typedef struct quantization
{
    qtype_t type;
    void* qConfig;
} quantization_t;

void initLinearQConfig(uint8_t qMax, roundingMode_t roundingMode, linearQConfig_t* linear_q);

void initInt32Quantization(quantization_t *quantization);
void initFloat32Quantization(quantization_t *quantization);
void initLinearQuantization(linearQConfig_t * linear_q,quantization_t *quantization);


#endif // ENV5_RUNTIME_QUANTIZATION_H
