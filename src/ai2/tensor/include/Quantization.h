#ifndef ENV5_RUNTIME_QUANTIZATION_H
#define ENV5_RUNTIME_QUANTIZATION_H
#include "Rounding.h"

typedef enum qtype
{
    INT32,
    FLOAT32,
    ASYM
} qtype_t;

/*! @brief Describes linear quantization
 * @param scale:
 * @param zeroPoint:
 * @param qMin:
 * @param qMax:
 */

typedef struct asymQConfig
{
    float scale;
    int16_t zeroPoint;
    uint8_t qBits;
    roundingMode_t roundingMode;
} asymQConfig_t;

typedef struct quantization
{
    qtype_t type;
    void* qConfig;
} quantization_t;

void initAsymQConfig(uint8_t qMax, roundingMode_t roundingMode, asymQConfig_t* asymQConfig);

void initInt32Quantization(quantization_t* quantization);
void initFloat32Quantization(quantization_t* quantization);
void initAsymQuantization(asymQConfig_t* asymQConfig, quantization_t* quantization);


#endif // ENV5_RUNTIME_QUANTIZATION_H
