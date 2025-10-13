#define SOURCE_FILE "AI-QUANTIZATION"

#include "Quantization.h"
#include "AiHelpers.h"
#include "math.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float16_t readBytesAsFloat16(uint8_t *startIndex) {
    float16_t dataType = (uint16_t)startIndex[0] | (uint16_t)startIndex[1] << 8;
    return dataType;
}

float32_t readBytesAsFloat32(uint8_t *startIndex) {
    uint32_t rawOutput = startIndex[0] | (uint32_t)startIndex[1] << 8 | (uint32_t)startIndex[2] <<
                         16 | (uint32_t)startIndex[3] << 24;
    float32_t f;
    memcpy(&f, &rawOutput, 4);
    return f;
}

float64_t readBytesAsFloat64(uint8_t *startIndex) {
    uint64_t rawOutput = startIndex[0] | (uint64_t)startIndex[1] << 8 | (uint64_t)startIndex[2] <<
                         16 | (uint64_t)startIndex[3] << 24
                         | (uint64_t)startIndex[4] << 32 | (uint64_t)startIndex[5] << 40 | (
                             uint64_t)startIndex[6] << 48 | (uint64_t)startIndex[7] << 56;
    float64_t f;
    memcpy(&f, &rawOutput, 8);
    return f;
}

quantization_t *initQuantizationByType(const qtype_t type) {
    quantization_t *quantization = calloc(1, sizeof(quantization_t));
    quantization->type = type;

    switch (type) {
    case FIXEDPOINT_Q:
        quantization->qConfig = calloc(1, sizeof(fixedPointQ_t));
        fixedPointQ_t *fixedConfig = quantization->qConfig;
        fixedConfig->signBit = 1;
        fixedConfig->intBits = 8;
        fixedConfig->fracBits = 7;
        break;
    case FLOAT16_Q:
        quantization->qConfig = calloc(1, sizeof(floatQ_t));
        floatQ_t *float16Config = quantization->qConfig;
        float16Config->mantissaBits = 8;
        float16Config->exponentBits = 8;
        break;
    case FLOAT32_Q:
        quantization->qConfig = calloc(1, sizeof(floatQ_t));

        floatQ_t *float32Config = quantization->qConfig;
        float32Config->signBit = 1;
        float32Config->exponentBits = 8;
        float32Config->mantissaBits = 23;
        break;
    case FLOAT64_Q:
        quantization->qConfig = calloc(1, sizeof(floatQ_t));

        floatQ_t *float64Config = quantization->qConfig;
        float64Config->signBit = 1;
        float64Config->exponentBits = 11;
        float64Config->mantissaBits = 52;
        break;
    case LINEAR_Q:
        quantization->qConfig = calloc(1, sizeof(linearQ_t));

        linearQ_t *affine8Config = quantization->qConfig;
        affine8Config->scale = 0;
        affine8Config->zeroPoint = 0;
        affine8Config->qMax = 0;
        affine8Config->qMin = 0;
    default:
        printf("Error: Unknown quantization type");
        break;
    }

    return quantization;
}

size_t calcBytesPerElement(quantization_t *quantization) {
    switch (quantization->type) {
    case FIXEDPOINT_Q:
        fixedPointQ_t *fixedConfig = quantization->qConfig;
        size_t bitsPerElement = fixedConfig->fracBits + fixedConfig->intBits + fixedConfig->signBit;
        return (bitsPerElement + 7) / sizeof(uint8_t);
    case FLOAT16_Q:
        return 2;
    case FLOAT32_Q:
        return 4;
    case FLOAT64_Q:
        return 8;
    case LINEAR_Q:
        return 1;
    default:
        printf("Error: unknown quantization type");
        return 0;
    }
}

qTensor_t *initQTensor(uint8_t *data, size_t numberOfDimensions, size_t *dimensions,
                       quantization_t *quantization) {
    qTensor_t *qTensor = calloc(1, sizeof(qTensor_t));

    size_t dataSize = calcTensorDataSize(numberOfDimensions, dimensions);

    size_t bytesPerElement = calcBytesPerElement(quantization);
    qTensor->data = calloc(dataSize, bytesPerElement);
    memcpy(qTensor->data, data, dataSize * bytesPerElement);

    qTensor->numberOfDimensions = numberOfDimensions;

    qTensor->dimensions = calloc(numberOfDimensions, sizeof(size_t));
    memcpy(qTensor->dimensions, dimensions, numberOfDimensions * sizeof(size_t));

    qTensor->quantization = quantization;

    return qTensor;
}

parameterQTensor_t *initParameterQTensor(uint8_t *data, size_t numberOfDimensions,
                                         size_t *dimensions, quantization_t *dataQuantization,
                                         quantization_t *gradQuantization) {
    parameterQTensor_t *parameterQTensor = calloc(1, sizeof(parameterQTensor_t));

    parameterQTensor->dataTensor = initQTensor(data, numberOfDimensions, dimensions,
                                               dataQuantization);

    size_t dataSize = calcTotalNumberOfElementsByTensor(parameterQTensor->dataTensor);
    float *gradData = calloc(dataSize, sizeof(uint8_t));
    parameterQTensor->gradTensor = initQTensor(gradData, numberOfDimensions, dimensions,
                                               gradQuantization);

    return parameterQTensor;
}

/*fixedPoint_t castFloatToFixed(fixedPointQ_t *fixedConfig, float input) {
    fixedPoint_t output = lroundf(input * (1 << fixedConfig->fracBits));
    return output;
}

float castFixedToFloat(fixedPointQ_t *fixedConfig, fixedPoint_t value) {
    unsigned int scale = 1u << fixedConfig->fracBits;
    return (float)value / (float)scale;
}

int16_t reconstruct16(uint8_t *bytes) {
    uint16_t u = (uint16_t)bytes[0] | (uint16_t)bytes[1] << 8;
    return u;
}*/
