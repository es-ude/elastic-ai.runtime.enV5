#include "Tensor.h"
#include "Rounding.h"

#include <stdlib.h>
#include <string.h>
#include <tgmath.h>


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

tensor_t *initTensor(tensor_t *tensor, uint8_t *data, quantization_t *quantization,
                     uint8_t *sparsityBitmask,
                     size_t numberOfDims, size_t *dims) {

    tensor->data = data;
    tensor->quantization = quantization;
    tensor->sparsityBitmask = sparsityBitmask;
    tensor->numberOfDimensions = numberOfDims;
    tensor->dimensions = dims;
    // TODO remove calloc
    tensor->orderOfDimensions = calloc(numberOfDims, sizeof(size_t));
    for (size_t i = 0; i < numberOfDims; i++) {
        tensor->orderOfDimensions[i] = i;
    }

    return tensor;
}

parameter_t *initParameter(tensor_t *tensor, uint8_t *data, quantization_t *dataQuantization,
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

void zeroTensorData(tensor_t *tensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(tensor);
    size_t bytesPerElement = calcBytesPerElement(tensor->quantization);
    memset(tensor->data, 0, numberOfElements * bytesPerElement);
}

// TODO auf orderOfDimensions ändern
void transposeTensor(const tensor_t *tensor, const size_t dim0, const size_t dim1) {
    tensor->dimensions[dim0] = dim1;
    tensor->dimensions[dim1] = dim0;
}

void convertFloatTensorToInt32Tensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    size_t inputBytesPerElement = calcBytesPerElement(inputTensor->quantization);
    size_t outputBytesPerElement = calcBytesPerElement(outputTensor->quantization);

    zeroTensorData(outputTensor);

    for (size_t element = 0; element < numberOfElements; element++) {
        size_t inputByteIndex = element * inputBytesPerElement;
        size_t outputByteIndex = element * outputBytesPerElement;

        float inputElement = readBytesAsFloat(&inputTensor->data[inputByteIndex]);
        int32_t outputElement = (int32_t)inputElement;

        memcpy(&outputTensor->data[outputByteIndex], &outputElement, outputBytesPerElement);
    }

    outputTensor->numberOfDimensions = inputTensor->numberOfDimensions;
    outputTensor->dimensions = inputTensor->dimensions;
    outputTensor->sparsityBitmask = inputTensor->sparsityBitmask;
}

void convertFloatTensorToLinearTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    size_t inputBytesPerElement = calcBytesPerElement(inputTensor->quantization);

    float max = findMaxFloat(inputTensor->data, numberOfElements, inputBytesPerElement);
    float min = findMinFloat(inputTensor->data, numberOfElements, inputBytesPerElement);

    linearQ_t *linearQConfig = outputTensor->quantization->qConfig;

    float scale = (max - min) / (float)linearQConfig->qMax;
    int16_t zeroPoint = roundByMode(-min / scale, linearQConfig->roundingMode);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t byteIndex = elementIndex * inputBytesPerElement;
        float inputElement = readBytesAsFloat(&inputTensor->data[byteIndex]);

        int32_t outputElement = (int32_t)roundf(inputElement / scale + zeroPoint);

        if (outputElement < 0)
            outputElement = 0;
        if (outputElement > 255)
            outputElement = 255;

        outputTensor->data[elementIndex] = (uint8_t)outputElement;
    }

    linearQConfig->scale = scale;
    linearQConfig->zeroPoint = zeroPoint;
}

void convertInt32TensorToFloatTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t bytesPerElement = 4;
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t byteIndex = elementIndex * bytesPerElement;

        int32_t inputElement = readBytesAsInt32(&inputTensor->data[byteIndex]);
        float outputElement = (float)inputElement;

        memcpy(&outputTensor->data[byteIndex], &outputElement, bytesPerElement);
    }
}

void convertInt32TensorToLinearTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    size_t inputBytesPerElement = calcBytesPerElement(inputTensor->quantization);
    size_t outputBytesPerElement = calcBytesPerElement(outputTensor->quantization);
    int32_t min = findMinInt32(inputTensor->data, numberOfElements, inputBytesPerElement);
    int32_t max = findMaxInt32(inputTensor->data, numberOfElements, inputBytesPerElement);

    linearQ_t *linearQConfig = outputTensor->quantization->qConfig;
    float scale = (float)(max - min) / (float)linearQConfig->qMax;
    int16_t zeroPoint = roundByMode(min / scale, linearQConfig->roundingMode);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t inputByteIndex = elementIndex * inputBytesPerElement;
        size_t outputByteIndex = elementIndex * outputBytesPerElement;

        int32_t inputElement = readBytesAsInt32(&inputTensor->data[inputByteIndex]);
        uint8_t outputElement = inputElement / (scale + zeroPoint);
        memcpy(&outputTensor->data[outputByteIndex], &outputElement, outputBytesPerElement);
    }
}

void convertLinearTensorToInt32Tensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    linearQ_t *linearQConfig = inputTensor->quantization->qConfig;
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    size_t inputBytesPerElement = calcBytesPerElement(inputTensor->quantization);
    size_t outputBytesPerElement = calcBytesPerElement(outputTensor->quantization);

    float scale = linearQConfig->scale;
    uint16_t zeroPoint = linearQConfig->zeroPoint;

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t inputByteIndex = elementIndex * inputBytesPerElement;
        size_t outputByteIndex = elementIndex * outputBytesPerElement;

        uint8_t inputElement = inputTensor->data[inputByteIndex];
        int32_t outputElement = scale * (inputElement - zeroPoint);
        memcpy(&outputTensor->data[outputByteIndex], &outputElement, outputBytesPerElement);
    }
}

void convertLinearTensorToFloatTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    linearQ_t *linearQConfig = inputTensor->quantization->qConfig;
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    size_t inputBytesPerElement = calcBytesPerElement(inputTensor->quantization);
    size_t outputBytesPerElement = calcBytesPerElement(outputTensor->quantization);

    float scale = linearQConfig->scale;
    int16_t zeroPoint = linearQConfig->zeroPoint;

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        size_t inputByteIndex = elementIndex * inputBytesPerElement;
        size_t outputByteIndex = elementIndex * outputBytesPerElement;

        uint8_t inputElement = inputTensor->data[inputByteIndex];
        float outputElement = (inputElement - zeroPoint) * scale;
        memcpy(&outputTensor->data[outputByteIndex], &outputElement, outputBytesPerElement);
    }
}

conversionFunction_t conversionMatrix[3][3] = {
    [INT32] = {NULL, convertInt32TensorToFloatTensor, convertInt32TensorToLinearTensor},
    [FLOAT32] = {convertFloatTensorToInt32Tensor, NULL, convertFloatTensorToLinearTensor},
    [LINEAR] = {convertLinearTensorToInt32Tensor, convertLinearTensorToFloatTensor, NULL}
};


void convertTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    dtype_t inputDType = inputTensor->quantization->type;
    dtype_t outputDType = outputTensor->quantization->type;
    conversionFunction_t conversionFn = conversionMatrix[inputDType][outputDType];
    conversionFn(inputTensor, outputTensor);
}

// TODO convert sparse tensor to tensor
// data wird größer (mit 0 auffüllen)
// Input tensor und bereits gecalloced data
