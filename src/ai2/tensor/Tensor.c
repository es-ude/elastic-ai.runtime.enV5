#include "Tensor.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

size_t calcNumberOfElementsByDims(size_t numberOfDimensions, size_t *dimensions) {
    size_t numElem = 1;
    for (size_t i = 0; i < numberOfDimensions; i++) {
        numElem *= dimensions[i];
    }
    return numElem;
}

size_t calcNumberOfElementsByTensor(tensor_t *qTensor) {
    size_t numElem = 1;
    for (size_t i = 0; i < qTensor->numberOfDimensions; i++) {
        numElem *= qTensor->dimensions[i];
    }
    return numElem;
}

size_t calcBytesPerElement(quantization_t *quantization) {
    switch (quantization->type) {
    case INT32:
        return sizeof(int32_t);
    case FLOAT32:
        return sizeof(float);
    case LINEAR:
        linearQ_t *linearQConfig = quantization->qConfig;
        return sizeof(linearQConfig->qMax);
    default:
        return 0;
    }
}

tensor_t *initTensor(uint8_t *data, quantization_t *quantization, uint8_t *sparsityBitmask,
                     size_t numberOfDims, size_t *dims) {
    tensor_t *tensor = calloc(1, sizeof(tensor_t));

    tensor->data = data;
    tensor->quantization = quantization;
    tensor->sparsityBitmask = sparsityBitmask;
    tensor->numberOfDimensions = numberOfDims;
    tensor->dimensions = dims;

    return tensor;
}

parameter_t *initParameter(uint8_t *data, quantization_t *dataQuantization,
                           uint8_t *sparsityBitmask, uint8_t *grad,
                           quantization_t *gradQuantization, size_t numberOfDims, size_t *dims) {
    parameter_t *parameter = calloc(1, sizeof(parameter_t));

    parameter->data = data;
    parameter->dataQuantization = dataQuantization;
    parameter->sparsityBitmask = sparsityBitmask;
    parameter->grad = grad;
    parameter->gradQuantization = gradQuantization;
    parameter->numberOfDimensions = numberOfDims;
    parameter->dimensions = dims;

    return parameter;
}

void convertTensorToInt32(tensor_t *tensor, linearTensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(tensor);
    size_t inputBytesPerElement = calcBytesPerElement(tensor->quantization);
    size_t outputBytesPerElement = calcBytesPerElement(outputTensor->quantization);

    outputTensor->data = calloc(numberOfElements, outputBytesPerElement);

    for (size_t element = 0; element < numberOfElements; element++) {
        size_t inputByteIndex = element * inputBytesPerElement;
        size_t outputByteIndex = element * outputBytesPerElement;
        memcpy(&outputTensor->data[outputByteIndex], &tensor->data[inputByteIndex],
               outputBytesPerElement);
    }

    outputTensor->numberOfDimensions = tensor->numberOfDimensions;
    outputTensor->dimensions = tensor->dimensions;
    outputTensor->sparsityBitmask = tensor->sparsityBitmask;
}


tensor_t *convertFloat32TensorToLinearTensor(float32Tensor_t *inputTensor, linearTensor_t *outputTensor) {
    linearQ_t *linearQConfig = outputTensor->quantization->qConfig;

    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    size_t inputBytesPerElement = calcBytesPerElement(inputTensor->quantization);

    float max = findMaxFloat(inputTensor->data, numberOfElements, inputBytesPerElement);
    float min = findMinFloat(inputTensor->data, numberOfElements, inputBytesPerElement);

    float scale = (max - min) / (float)linearQConfig->qMax;
    uint16_t zeroPoint;

    switch (linearQConfig->roundingMode) {
    case HTE:
        zeroPoint = ceil(min / scale);
        break;
    case SRHTE:
        break;
    }

}
