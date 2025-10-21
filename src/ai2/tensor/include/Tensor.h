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
    size_t* orderOfDimensions;
} tensor_t;

#include <stdint.h>


typedef struct int32Tensor
{
    uint8_t* data;
    quantization_t* quantization = initQuantization(INT32);
    uint8_t* sparsityBitmask;
    size_t numberOfDimensions;
    size_t* dimensions;
    size_t* orderOfDimensions;
} int32Tensor_t;

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

typedef void(*conversionFunction_t)(tensor_t *inputTensor, tensor_t *outputTensor);

tensor_t* initTensor(uint8_t* data, quantization_t* quantization, uint8_t* sparsityBitmask, size_t numberOfDims,
                     size_t* dims);
parameter_t* initParameter(uint8_t* data, quantization_t* dataQuantization, uint8_t* sparsityBitmask, uint8_t* grad,
                           quantization_t* gradQuantization, size_t numberOfDims, size_t* dims);

size_t calcBytesPerElement(quantization_t *quantization);

void transposeTensor(const tensor_t* tensor, size_t dim0, size_t dim1);

/*! @brief Converts given input tensor into given output tensor
 * @param inputTensor: input tensor
 * @param outputTensor: output tensor with wanted quantization
 */
void convertTensor(tensor_t *inputTensor, tensor_t *outputTensor);

#endif // ELASTIC_AI_RUNTIME_ENV5_TENSOR_H
