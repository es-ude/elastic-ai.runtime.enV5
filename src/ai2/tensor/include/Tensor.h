//
// Created by Leo Buron on 20.10.25.
//

#ifndef ELASTIC_AI_RUNTIME_ENV5_TENSOR_H
#define ELASTIC_AI_RUNTIME_ENV5_TENSOR_H

#include "DTypes.h"
#include <stddef.h>
#include <stdint.h>


typedef void* tensorStorageId;


typedef struct Tensor
{
    uint8_t* data;
    encoding_t* encoding;
    uint8_t* sparsityBitmask;
    size_t numberOfDimensions;
    size_t* dimensions;
} tensor_t;

typedef struct Parameter
{
    uint8_t* data;
    encoding_t* encoding;
    uint8_t* sparsityBitmask;
    uint8_t* dataGrad;
    encoding_t* encodingGrad;
    size_t numberOfDimensions;
    size_t* dimensions;
} parameter_t;



void convertTensorToInt32(tensor_t* tensor, tensor_t* outputTensor);
void convertTensorToFloat32(tensor_t* tensor, tensor_t* outputTensor);
void convertTensorToLinearQ(tensor_t* tensor, tensor_t* outputTensor);


#endif // ELASTIC_AI_RUNTIME_ENV5_TENSOR_H
