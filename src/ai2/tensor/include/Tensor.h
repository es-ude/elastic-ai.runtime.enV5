#ifndef ELASTIC_AI_RUNTIME_ENV5_TENSOR_H
#define ELASTIC_AI_RUNTIME_ENV5_TENSOR_H

#include "Quantization.h"

#include <stddef.h>
#include <stdint.h>

typedef void* tensorStorageId;

typedef struct shape
{
    size_t numberOfDimensions;
    size_t* dimensions;
    size_t* orderOfDimensions;
} shape_t;

typedef struct Tensor
{
    uint8_t* data;
    shape_t shape;
    quantization_t* quantization;
    uint8_t* sparsityBitmask;
} tensor_t;

typedef struct Parameter
{
    tensor_t tensor;
    uint8_t* grad;
    quantization_t* gradQuantization;

} parameter_t;

uint32_t getBitmask(uint32_t startbit, uint32_t endbit);

uint8_t writeByte(uint8_t existingData, uint8_t data, uint8_t startbit, uint8_t endbit);

uint8_t readByte(uint8_t data, uint8_t startbit, uint8_t endbit);

void byteConversion(uint8_t* dataIn, size_t dataInBits, uint8_t* dataOut, size_t dataOutBits, size_t numValues);


void initTensor(tensor_t* tensor, uint8_t* data, quantization_t* quantization, uint8_t* sparsityBitmask,
                size_t numberOfDims,
                size_t* dims, size_t* orderOfDimensions);
void initParameter(parameter_t* parameter, uint8_t* data, quantization_t* dataQuantization, uint8_t* sparsityBitmask,
                   uint8_t* grad,
                   quantization_t* gradQuantization, size_t numberOfDims, size_t* dims);

void getTensorFromParameter(parameter_t* parameter, tensor_t* tensor, size_t* orderOfDimensions);

void getGradTensorFromParameter(parameter_t* parameter, tensor_t* tensor, size_t* orderOfDimensions);

size_t calcBytesPerElement(quantization_t* quantization);

size_t calcNumberOfElementsByDims(size_t numberOfDimensions, size_t* dimensions);

size_t calcBitsPerElement(quantization_t* quantization);

size_t calcBytesPerTensor(tensor_t* tensor);

size_t calcNumberOfElementsByTensor(tensor_t* tensor);

size_t calcNumberOfElementsByParameter(parameter_t *parameter);

void transposeTensor(tensor_t* tensor, size_t dim0Index, size_t dim1Index);

void setOrderOfDimsForNewTensor(size_t numberOfDimensions, size_t *orderOfDimensions);


void setTensorValuesForConversion(uint8_t* data, quantization_t* q, tensor_t* originalTensor, tensor_t* outputTensor);

void setTensorValues(tensor_t* tensor, uint8_t* data, size_t* dims, size_t numberOfDims,
                     size_t* orderOfDims,
                     quantization_t* quantization, uint8_t* sparsityBitmask);

void setParameterValues(parameter_t* parameter, uint8_t* data, quantization_t* dataQuantization, uint8_t* grad, quantization_t* gradQuantization, size_t* dims, size_t numberOfDims,
                     size_t* orderOfDims, uint8_t* sparsityBitmask);

void printTensor(tensor_t *t);

void initOrderOfDimensions(size_t *orderOfDims, size_t numberOfDims);

#endif // ELASTIC_AI_RUNTIME_ENV5_TENSOR_H
