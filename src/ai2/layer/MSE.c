#include "MSE.h"
#include "Tensor.h"
#include "TensorConversion.h"
#include "Sub.h"

#include <Mul.h>
#include <stdio.h>

void MSELossBackwardFloat(tensor_t *modelOutput, tensor_t *label, tensor_t *result) {
    size_t numberOfElements = calcNumberOfElementsByTensor(modelOutput);

    float mean = 2.f / (float)numberOfElements;

    float *modelOutputArray = (float *)modelOutput->data;
    float *labelArray = (float *)label->data;
    float *resultArray = (float *)result->data;

    for (size_t i = 0; i < numberOfElements; i++) {
        resultArray[i] = mulFloat32s(mean, subFloat32s(modelOutputArray[i], labelArray[i]));
    }
}

void MSELossBackwardAsym(tensor_t *modelOutput, tensor_t *label, tensor_t *result) {
    size_t numberOfElements = calcNumberOfElementsByTensor(modelOutput);

    tensor_t modelOutputFloat;
    quantization_t modelOutputFloatQ;
    initFloat32Quantization(&modelOutputFloatQ);
    float modelOutputFloatData[numberOfElements];
    setTensorValuesForConversion(modelOutputFloatData, &modelOutputFloatQ, modelOutput,
                                 &modelOutputFloat);
    convertTensor(modelOutput, &modelOutputFloat);

    tensor_t labelFloat;
    quantization_t labelFloatQ;
    initFloat32Quantization(&labelFloatQ);
    float labelFloatData[numberOfElements];
    setTensorValuesForConversion(labelFloatData, &labelFloatQ, label, &labelFloat);
    convertTensor(label, &labelFloat);

    tensor_t resultFloat;
    quantization_t resultFloatQ;
    initFloat32Quantization(&resultFloatQ);
    float resultFloatData[numberOfElements];
    setTensorValuesForConversion(resultFloatData, &resultFloatQ, result, &resultFloat);
    convertTensor(result, &resultFloat);

    float *modelOutputArray = (float *)modelOutputFloat.data;
    float *labelArray = (float *)labelFloat.data;
    float *resultArray = (float *)resultFloat.data;

    float mean = 2.f / (float)numberOfElements;

    for (size_t i = 0; i < numberOfElements; i++) {
        resultArray[i] = mulFloat32s(mean, subFloat32s(modelOutputArray[i], labelArray[i]));
    }

    convertTensor(&resultFloat, result);
}

void MSELossBackward(tensor_t *modelOutput, tensor_t *label, tensor_t *result) {
    qtype_t modelOutputQType = modelOutput->quantization->type;

    switch (modelOutputQType) {
    case FLOAT32:
        MSELossBackwardFloat(modelOutput, label, result);
        break;
    case ASYM:
        MSELossBackwardAsym(modelOutput, label, result);
        break;
    default:
        printf("Error in MSE Backward: qtype not supported\n");
        break;
    }
}
