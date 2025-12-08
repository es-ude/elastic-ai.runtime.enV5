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

typedef struct tensor
{
    uint8_t* data;
    shape_t* shape;
    quantization_t* quantization;
    uint8_t* sparsityBitmask;
} tensor_t;

typedef struct parameter
{
    tensor_t* param;
    tensor_t* grad;
} parameter_t;

uint32_t getBitmask(uint32_t startbit, uint32_t endbit);

uint8_t writeByte(uint8_t existingData, uint8_t data, uint8_t startbit, uint8_t endbit);

uint8_t readByte(uint8_t data, uint8_t startbit, uint8_t endbit);

void byteConversion(uint8_t* dataIn, size_t dataInBits, uint8_t* dataOut, size_t dataOutBits, size_t numValues);

tensor_t* getTensorFromParameter(parameter_t* parameter);
tensor_t* getGradTensorFromParameter(parameter_t* parameter);

size_t calcBytesPerElement(quantization_t* quantization);
size_t calcBitsPerElement(quantization_t* quantization);
size_t calcBytesPerTensor(tensor_t* tensor);

size_t calcNumberOfElementsByShape(shape_t* shape);
size_t calcNumberOfElementsByTensor(tensor_t* tensor);
size_t calcNumberOfElementsByParameter(parameter_t* parameter);

void transposeTensor(tensor_t* tensor, size_t dim0Index, size_t dim1Index);

void setTensorValuesForConversion(uint8_t* data, quantization_t* q, tensor_t* originalTensor, tensor_t* outputTensor);
void setTensorValues(tensor_t* tensor, uint8_t* data, shape_t* shape, quantization_t* quantization,
                     uint8_t* sparsityBitmask);
void setParameterValues(parameter_t* parameter, tensor_t* param, tensor_t* grad);
void setOrderOfDimsForNewTensor(size_t numberOfDimensions, size_t* orderOfDimensions);
void setShape(shape_t *shape, size_t *dims, size_t numberOfDims, size_t *orderOfDims);

void printTensor(tensor_t* t);
void printShape(shape_t* shape);

void copyTensor(tensor_t* dest, tensor_t* src);

#endif // ELASTIC_AI_RUNTIME_ENV5_TENSOR_H
