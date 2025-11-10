#ifndef ENV5_RUNTIME_TENSORCONVERSION_H
#define ENV5_RUNTIME_TENSORCONVERSION_H
#include "Tensor.h"
typedef void (*conversionFunction_t)(tensor_t* inputTensor, tensor_t* outputTensor);

/*! @brief Converts given input tensor into given output tensor
 * @param inputTensor: input tensor
 * @param outputTensor: output tensor with wanted quantization
 */
void convertTensor(tensor_t* inputTensor, tensor_t* outputTensor);

#endif // ENV5_RUNTIME_TENSORCONVERSION_H
