#include "Relu.h"
#include "Tensor.h"

#include <DTypes.h>
#include <stdio.h>

void reluForwardFloat32(tensor_t *input, tensor_t *output) {
    size_t numberOfElements = calcNumberOfElementsByTensor(input);

    tensor_t intermediateOutput;
    quantization_t intermediateOutputQ;

    initFloat32Quantization(&intermediateOutputQ);
    uint8_t intermediateOutputFloatData[numberOfElements * sizeof(float)];
    setTensorValues(&intermediateOutput, intermediateOutputFloatData, output->dimensions,
                    output->numberOfDimensions, output->orderOfDimensions, &intermediateOutputQ,
                    output->sparsityBitmask);

    float floatArray[numberOfElements];
    readBytesAsFloatArray(numberOfElements, input->data, floatArray);

    for (size_t i = 0; i < numberOfElements; i++) {
        size_t byteIndex = i * sizeof(float);
        float currentElement = floatArray[i];
        if (currentElement < 0) {
            currentElement = 0;
        }

        writeFloatToByteArray(currentElement, &intermediateOutput.data[byteIndex]);
    }

    convertTensor(&intermediateOutput, output);

}

void reluForwardInt32(tensor_t *input, tensor_t *output) {
    size_t numberOfElements = calcNumberOfElementsByTensor(input);

    tensor_t intermediateOutput;
    quantization_t intermediateOutputQ;
    initInt32Quantization(&intermediateOutputQ);
    int32_t intermediateOutputIntData[numberOfElements];
    setTensorValues(&intermediateOutput, intermediateOutputIntData, output->dimensions,
                    output->numberOfDimensions, output->orderOfDimensions, &intermediateOutputQ,
                    output->sparsityBitmask);

    int32_t intArray[numberOfElements];
    readBytesAsInt32Array(numberOfElements, input->data, intArray);

    for (size_t i = 0; i < numberOfElements; i++) {
        size_t byteIndex = i * sizeof(int32_t);
        int32_t currentElement = intArray[i];
        if (currentElement < 0) {
            currentElement = 0;
        }
        writeInt32ToByteArray(currentElement, &intermediateOutput.data[byteIndex]);
    }

    convertTensor(&intermediateOutput, output);
}

// todo change conversion to int
void reluForwardAsym(tensor_t *input, tensor_t *output) {

    size_t numberOfElements = calcNumberOfElementsByTensor(input);

    tensor_t intermediateInputFloat;
    quantization_t intermediateInputFloatQ;
    initFloat32Quantization(&intermediateInputFloatQ);
    uint8_t intermediateInputFloatData[numberOfElements * sizeof(float)];
    setTensorValues(&intermediateInputFloat, intermediateInputFloatData, input->dimensions,
                    input->numberOfDimensions, input->orderOfDimensions, &intermediateInputFloatQ,
                    NULL);
    convertTensor(input, &intermediateInputFloat);

    tensor_t intermediateOutput;
    quantization_t intermediateOutputQ;
    initFloat32Quantization(&intermediateOutputQ);
    uint8_t intermediateOutputData[numberOfElements * sizeof(float)];
    setTensorValues(&intermediateOutput, intermediateOutputData, output->dimensions,
                    output->numberOfDimensions, output->orderOfDimensions, &intermediateOutputQ,
                    output->sparsityBitmask);

    reluForwardFloat32(&intermediateInputFloat, &intermediateOutput);

    convertTensor(&intermediateOutput, output);
}

void reluForward(void *config, tensor_t *input, tensor_t *output) {
    qtype_t inputQType = input->quantization->type;

    switch (inputQType) {
    case FLOAT32:
        reluForwardFloat32(input, output);
        break;

    case INT32:
        reluForwardInt32(input, output);
        break;

    case ASYM:
        reluForwardAsym(input, output);
        break;

    default:
        break;
    }
}


void reluBackwardFloat32(tensor_t *input, tensor_t *gradOutputFromPreviousLayer, tensor_t *gradInput) {
    size_t numberOfElements = calcNumberOfElementsByTensor(input);
    float inputArray[numberOfElements];
    float gradOutArray[numberOfElements];
    readBytesAsFloatArray(numberOfElements, input->data, inputArray);
    readBytesAsFloatArray(numberOfElements, gradOutputFromPreviousLayer->data, gradOutArray);

    for (size_t i = 0; i < numberOfElements; i++) {
        float currentElement = gradOutArray[i];
        if(inputArray[i] <= 0) {
            currentElement = 0;
        }
        writeFloatToByteArray(currentElement, &gradInput->data[i * sizeof(float)]);
    }
}

void reluBackwardInt32(tensor_t *input, tensor_t *gradOutputFromPreviousLayer, tensor_t *gradInput) {
    size_t numberOfElements = calcNumberOfElementsByTensor(input);
    int32_t inputArray[numberOfElements];
    int32_t gradOutArray[numberOfElements];
    readBytesAsInt32Array(numberOfElements, input->data, inputArray);
    readBytesAsInt32Array(numberOfElements, gradOutputFromPreviousLayer->data, gradOutArray);

    for (size_t i = 0; i < numberOfElements; i++) {
        int32_t currentElement = gradOutArray[i];
        if(inputArray[i] <= 0) {
            currentElement = 0;
        }
        writeInt32ToByteArray(currentElement, &gradInput->data[i * sizeof(int32_t)]);
    }
}

// TODO change q from float to int
void reluBackwardAsym(tensor_t *input, tensor_t *gradOutput, tensor_t *gradInput) {
    size_t numberOfElements = calcNumberOfElementsByTensor(input);

    float inputFloatData[numberOfElements];
    quantization_t inputFloatQ;
    initFloat32Quantization(&inputFloatQ);
    tensor_t inputFloat;
    setTensorValuesForConversion(inputFloatData, &inputFloatQ, input, &inputFloat);
    convertTensor(input, &inputFloat);

    float gradOutputFloatData[numberOfElements];
    quantization_t gradOutputFloatQ;
    initFloat32Quantization(&gradOutputFloatQ);
    tensor_t gradOutputFloat;
    setTensorValuesForConversion(gradOutputFloatData, &gradOutputFloatQ, gradOutput, &gradOutputFloat);
    convertTensor(gradOutput, &gradOutputFloat);

    float gradInputFloatData[numberOfElements];
    quantization_t gradInputFloatQ;
    initFloat32Quantization(&gradInputFloatQ);
    tensor_t gradInputFloat;
    setTensorValuesForConversion(gradInputFloatData, &gradInputFloatQ, gradInput, &gradInputFloat);

    reluBackwardFloat32(&inputFloat, &gradOutputFloat, &gradInputFloat);

    convertTensor(&gradInputFloat, gradInput);
}

void reluBackward(void *config, tensor_t *input, tensor_t *gradOutputFromPreviousLayer, tensor_t *gradInput) {
    qtype_t inputQType = input->quantization->type;

    switch (inputQType) {
    case FLOAT32:
        reluBackwardFloat32(input, gradOutputFromPreviousLayer, gradInput);
        break;
    case INT32:
        reluBackwardInt32(input, gradOutputFromPreviousLayer, gradInput);
        break;
    case ASYM:
        reluBackwardAsym(input, gradOutputFromPreviousLayer, gradInput);
        break;
    default:
        break;
    }
}




