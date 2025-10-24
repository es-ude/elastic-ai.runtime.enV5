#ifndef ELASTIC_AI_RUNTIME_ENV5_TENSOR_H
#define ELASTIC_AI_RUNTIME_ENV5_TENSOR_H

#include "Quantization.h"

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

uint32_t getBitmask(uint32_t startbit, uint32_t endbit);

uint8_t writeByte(uint8_t existingData, uint8_t data, uint8_t startbit, uint8_t endbit);

uint8_t readByte(uint8_t data, uint8_t startbit, uint8_t endbit);

void byteConversion(uint8_t *dataIn, size_t dataInBits, uint8_t *dataOut, size_t dataOutBits, size_t numValues);

typedef void(*conversionFunction_t)(tensor_t *inputTensor, tensor_t *outputTensor);

void initTensor(tensor_t *tensor, uint8_t* data, quantization_t* quantization, uint8_t* sparsityBitmask, size_t numberOfDims,
                     size_t* dims, size_t *orderOfDimensions);
void initParameter(parameter_t *parameter, uint8_t* data, quantization_t* dataQuantization, uint8_t* sparsityBitmask, uint8_t* grad,
                           quantization_t* gradQuantization, size_t numberOfDims, size_t* dims);

void getTensorFromParameter(parameter_t *parameter, tensor_t *tensor, size_t *orderOfDimensions);

void getGradTensorFromParameter(parameter_t *parameter, tensor_t *tensor, size_t *orderOfDimensions);

size_t calcBytesPerElement(quantization_t *quantization);


size_t calcNumberOfElementsByDims(size_t numberOfDimensions, size_t *dimensions);

size_t calcBitsPerElement(quantization_t *quantization);

size_t calcBytesPerTensor(tensor_t *tensor);

size_t calcNumberOfElementsByTensor(tensor_t *qTensor);

void transposeTensor(tensor_t* tensor, size_t dim0Index, size_t dim1Index);

/*! @brief Converts given input tensor into given output tensor
 * @param inputTensor: input tensor
 * @param outputTensor: output tensor with wanted quantization
 */
void convertTensor(tensor_t *inputTensor, tensor_t *outputTensor);

float readTensorElementAsFloatByByteIndex(tensor_t *inputTensor, size_t elementByteIndex);

float readTensorElementAsFloatByIndex(tensor_t *inputTensor, size_t Index);

#endif // ELASTIC_AI_RUNTIME_ENV5_TENSOR_H
