#ifndef ELASTIC_AI_RUNTIME_ENV5_TENSOR_H
#define ELASTIC_AI_RUNTIME_ENV5_TENSOR_H

#include "DTypes.h"
#include <stddef.h>
#include <stdint.h>


typedef void* tensorStorageId;


typedef struct Tensor
{
    uint8_t* data;
    quantization_t* quantization;
    uint8_t* sparsityBitmask;
    size_t numberOfDimensions;
    size_t* dimensions;
} tensor_t;

typedef struct Parameter
{
    uint8_t* data;
    quantization_t* dataQuantization;
    uint8_t* sparsityBitmask;
    uint8_t* grad;
    quantization_t* gradQuantization;
    size_t numberOfDimensions;
    size_t* dimensions;
} parameter_t;

tensor_t* initTensor(uint8_t* data, quantization_t* quantization, uint8_t* sparsityBitmask, size_t numberOfDims,
                     size_t* dims);
parameter_t* initParameter(uint8_t* data, quantization_t* dataQuantization, uint8_t* sparsityBitmask, uint8_t* grad,
                           quantization_t* gradQuantization, size_t numberOfDims, size_t* dims);

typedef tensor_t float32Tensor_t = {
    .quantization = initQuantization(FLOAT32)
};

typedef tensor_t int32Tensor_t = {
    .quantization = initQuantization(INT32)
};

typedef tensor_t linearTensor_t = {
    .quantization = initQuantization(LINEAR)
};

void convertTensorToInt32(tensor_t* tensor, int32Tensor_t* outputTensor);
void convertTensorToFloat32(tensor_t* tensor, float32Tensor_t* outputTensor);
void convertTensorToLinearQ(tensor_t* tensor, linearTensor_t* outputTensor);


#endif // ELASTIC_AI_RUNTIME_ENV5_TENSOR_H
