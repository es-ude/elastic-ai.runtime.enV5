#include "Div.h"
#include "Arithmetic.h"
#include "DTypes.h"

#include <stdio.h>

#ifdef TRACK_INSTRUCTIONS
#define DIV_FUNC_INT divIntsWithInstructionCounter
#define DIV_FUNC_FLOAT divFloatsWithInstructionCounter
#else
#define DIV_FUNC_INT divInts
#define DIV_FUNC_FLOAT divFloats
#endif

size_t divInstructionCounter = 0;

int32_t divInts(int32_t a, int32_t b) {
    return a / b;
}

int32_t divIntsWithInstructionCounter(int32_t a, int32_t b) {
    ++divInstructionCounter;
    return a / b;
}

int32_t divInt32s(int32_t a, int32_t b){
    return DIV_FUNC_INT(a, b);
}

float divFloats(float a, float b) {
    return a / b;
}

float divFloatsWithInstructionCounter(float a, float b) {
    ++divInstructionCounter;
    return a / b;
}

float divFloat32s(float a, float b) {
    return DIV_FUNC_FLOAT(a, b);
}

void divInt32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    int32PointWiseArithmetic(a, b, DIV_FUNC_INT, outputTensor);
}

void divInt32TensorsInplace(tensor_t *a, tensor_t *b) {
    int32PointWiseArithmeticInplace(a, b, DIV_FUNC_INT);
}

void divInt32ElementWithInt32Tensor(tensor_t *a, int32_t b, tensor_t *outputTensor) {
    int32ElementWithTensorArithmetic(a, b, DIV_FUNC_INT, outputTensor);
}

void divInt32ElementWithInt32TensorInplace(tensor_t *a, int32_t b) {
    int32ElementWithTensorArithmeticInplace(a, b, DIV_FUNC_INT);
}

void divFloat32Tensors(tensor_t *a, tensor_t *b, tensor_t *outputTensor) {
    floatPointWiseArithmetic(a, b, DIV_FUNC_FLOAT, outputTensor);
}

void divFloat32TensorsInplace(tensor_t *a, tensor_t *b) {
    floatPointWiseArithmeticInplace(a, b, DIV_FUNC_FLOAT);
}

void divFloat32ElementWithFloat32Tensor(tensor_t *a, float b, tensor_t *outputTensor) {
    floatElementWithTensorArithmetic(a, b, DIV_FUNC_FLOAT, outputTensor);
}

void divFloat32ElementWithFloat32TensorInplace(tensor_t *a, float b) {
    floatElementWithTensorArithmeticInplace(a, b, DIV_FUNC_FLOAT);
}

void divSymInt32Tensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(aTensor);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(bTensor);

    if (aNumberOfValues != bNumberOfValues) {
        printf("Error in DivSymInt32Tensors: mismatched number of values\n");
    }

    int32_t aValues[aNumberOfValues];
    int32_t bValues[aNumberOfValues];
    readBytesAsInt32Array(aNumberOfValues, aTensor->data, aValues);
    readBytesAsInt32Array(aNumberOfValues, bTensor->data, bValues);

    int32_t output[aNumberOfValues];
    for (size_t i = 0; i < aNumberOfValues; i++) {
        output[i] = DIV_FUNC_INT(aValues[i], bValues[i]);
    }

    symInt32QConfig_t *aConfig = aTensor->quantization->qConfig;
    symInt32QConfig_t *bConfig = bTensor->quantization->qConfig;

    writeInt32ArrayToByteArray(aNumberOfValues, output, outputTensor->data);
    symInt32QConfig_t *outputConfig = outputTensor->quantization->qConfig;
    outputConfig->scale = DIV_FUNC_FLOAT(aConfig->scale, bConfig->scale);
}

void divSymInt32TensorsInplace(tensor_t *aTensor, tensor_t *bTensor) {
    size_t aNumberOfValues = calcNumberOfElementsByTensor(aTensor);
    size_t bNumberOfValues = calcNumberOfElementsByTensor(bTensor);

    if (aNumberOfValues != bNumberOfValues) {
        printf("Error in DivSymInt32TensorsInplace: mismatched number of values\n");
    }

    int32_t aValues[aNumberOfValues];
    int32_t bValues[aNumberOfValues];
    readBytesAsInt32Array(aNumberOfValues, aTensor->data, aValues);
    readBytesAsInt32Array(aNumberOfValues, bTensor->data, bValues);

    int32_t output[aNumberOfValues];
    for (size_t i = 0; i < aNumberOfValues; i++) {
        output[i] = DIV_FUNC_INT(aValues[i], bValues[i]);
    }

    writeInt32ArrayToByteArray(aNumberOfValues, output, aTensor->data);

    symInt32QConfig_t *aConfig = aTensor->quantization->qConfig;
    symInt32QConfig_t *bConfig = bTensor->quantization->qConfig;

    aConfig->scale = DIV_FUNC_FLOAT(aConfig->scale, bConfig->scale);
}

size_t getDivInstructionCounter() {
    return divInstructionCounter;
}