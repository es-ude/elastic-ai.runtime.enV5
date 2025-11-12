#ifndef ENV5_RUNTIME_QUANTIZATION_H
#define ENV5_RUNTIME_QUANTIZATION_H
#include "Rounding.h"

typedef enum qtype
{
    INT32,
    FLOAT32,
    SYM_INT32,
    SYM,
    ASYM
} qtype_t;


typedef struct symInt32QConfig
{
    float scale;
    roundingMode_t roundingMode;
} symInt32QConfig_t;

typedef struct symQConfig
{
    float scale;
    uint8_t qBits;
    roundingMode_t roundingMode;
} symQConfig_t;

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


void initSymInt32QConfig(roundingMode_t roundingMode, symInt32QConfig_t* symInt32QConfig);
void initSymQConfig(uint8_t qMax, roundingMode_t roundingMode, symQConfig_t* symQConfig);
void initAsymQConfig(uint8_t qMax, roundingMode_t roundingMode, asymQConfig_t* asymQConfig);


void initInt32Quantization(quantization_t* quantization);
void initFloat32Quantization(quantization_t* quantization);


void initSymInt32Quantization(symInt32QConfig_t* symInt32QConfig, quantization_t* quantization);
void initSymQuantization(symQConfig_t* symQConfig, quantization_t* quantization);
void initAsymQuantization(asymQConfig_t* asymQConfig, quantization_t* quantization);

#endif // ENV5_RUNTIME_QUANTIZATION_H
