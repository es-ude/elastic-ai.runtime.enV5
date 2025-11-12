#ifndef ENV5_RUNTIME_TENSOR_MATH_H
#define ENV5_RUNTIME_TENSOR_MATH_H
#include "Tensor.h"

#include <stdbool.h>


typedef int32_t(*int32ElementArithmeticFunc_t)(int32_t a, int32_t b);
typedef float(*floatElementArithmeticFunc_t)(float a, float b);

bool doDimensionsMatch(tensor_t* a, tensor_t* b);

void orderDims(tensor_t *tensor, size_t *orderedDims);

size_t getDimensionsByIndex(tensor_t *tensor, size_t index);
size_t calcTensorIndexByIndices(size_t numberOfDimensions, size_t *dimensions, size_t *indices);

void calcIndicesByRawIndex(size_t numberOfDims, size_t *dims, size_t rawIndex, size_t *indices);

size_t calcElementIndexByIndices(size_t numberOfDims, size_t *dims, size_t *indices, size_t *orderOfDimensions);

void int32PointWiseArithmetic(tensor_t *aTensor, tensor_t *bTensor, int32ElementArithmeticFunc_t arithmeticFunc, tensor_t *outputTensor);
void floatPointWiseArithmetic(tensor_t *aTensor, tensor_t *bTensor, floatElementArithmeticFunc_t arithmeticFunc, tensor_t *outputTensor);

void int32PointWiseArithmeticInplace(tensor_t *aTensor, tensor_t *bTensor, int32ElementArithmeticFunc_t arithmeticFunc);
void floatPointWiseArithmeticInplace(tensor_t *aTensor, tensor_t *bTensor, floatElementArithmeticFunc_t arithmeticFunc);

void int32ElementWithTensorArithmetic(tensor_t *aTensor, int32_t x, int32ElementArithmeticFunc_t arithmeticFunc, tensor_t *outputTensor);
void floatElementWithTensorArithmetic(tensor_t *aTensor, float x, floatElementArithmeticFunc_t arithmeticFunc, tensor_t *outputTensor);

void int32ElementWithTensorArithmeticInplace(tensor_t *aTensor, int32_t x, int32ElementArithmeticFunc_t arithmeticFunc);
void floatElementWithTensorArithmeticInplace(tensor_t *aTensor, float x, floatElementArithmeticFunc_t arithmeticFunc);

#endif // ENV5_RUNTIME_TENSOR_MATH_H
