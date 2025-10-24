#include "Tensor.h"
#include "Rounding.h"
#include "Quantization.h"
#include "MinMax.h"
#include "DTypes.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tgmath.h>

size_t calcNumberOfElementsByDims(size_t numberOfDimensions, size_t *dimensions) {
    size_t numElem = 1;
    for (size_t i = 0; i < numberOfDimensions; i++) {
        numElem *= dimensions[i];
    }
    return numElem;
}

size_t calcNumberOfElementsByTensor(tensor_t *tensor) {
    return calcNumberOfElementsByDims(tensor->numberOfDimensions, tensor->dimensions);
}

size_t calcBytesPerElement(quantization_t *quantization) {
    switch (quantization->type) {
    case INT32:
        return sizeof(int32_t);
    case FLOAT32:
        return sizeof(float);
    case LINEAR:
        linearQConfig_t *linearQConfig = quantization->qConfig;
        const uint32_t qBits = (uint32_t)linearQConfig->qBits;
        return qBits / 8;
    default:
        return 0;
    }
}

size_t calcBitsPerElement(quantization_t *quantization) {
    switch (quantization->type) {
        case INT32:
            return sizeof(int32_t)*8;
        case FLOAT32:
            return sizeof(float)*8;
        case LINEAR:
            linearQConfig_t *linearQConfig = quantization->qConfig;
            return linearQConfig->qBits;
        default:
            return 0;
    }
}

size_t calcBitsPerTensor(tensor_t *tensor) {
    size_t bitsPerElement = calcBitsPerElement(tensor->quantization);
    size_t numElements = calcNumberOfElementsByDims(tensor->numberOfDimensions, tensor->dimensions);
    return bitsPerElement * numElements;
}

size_t calcBytesPerTensor(tensor_t *tensor) {
    size_t bitsPerTensor = calcBitsPerTensor(tensor);
    return bitsPerTensor/8;
}

void setOrderOfDimsForNewTensor(size_t numberOfDimensions, size_t* orderOfDimensions) {
    for (size_t i = 0; i < numberOfDimensions; i++) {
        orderOfDimensions[i] = i;
    }
}

void print_binary_uint8(uint8_t x)
{
    /* Show the most‑significant bit first */
    printf("Byte ");
    for (int i = 7; i >= 0; --i) {
        putchar((x >> i) & 1 ? '1' : '0');
    }
    putchar('\n');   /* newline for convenience */
}

uint32_t getBitmask(uint32_t startbit, uint32_t endbit) {
    uint32_t endbitInternal = endbit - (startbit/8)*8;
    uint32_t startbitInternal = startbit - (startbit/8)*8;
    uint32_t counter = 0;
    uint32_t value = 1;
    for (size_t i = 0; i < 8; i++) {
        if ((i>=startbitInternal) & (endbitInternal > i)) {
            counter += value;
        }
        value *= 2;
    }
    //printf("bitmask ");
    //print_binary_uint8(counter);
    return counter;
}
uint8_t readByte(uint8_t data, uint8_t startbit, uint8_t endbit) {
    uint8_t bitmask = getBitmask(startbit, endbit);
    uint8_t intermediate = data & bitmask;
    intermediate >>= startbit-(startbit/8)*8;
    return intermediate;
}

uint8_t writeByte(uint8_t existingData, uint8_t data, uint8_t startbit, uint8_t endbit) {
    uint8_t startbitInternal = startbit - (startbit/8)*8;
    uint8_t endbitInternal = endbit - (startbit/8)*8;
    uint8_t bitmask = getBitmask(startbitInternal, endbitInternal);
    data <<= startbitInternal;
    //print_binary_uint8(data);
    uint8_t intermediate = data & bitmask;
    //print_binary_uint8(bitmask);
    //print_binary_uint8(intermediate);
    existingData = intermediate | existingData;
    //print_binary_uint8(existingData);
    return existingData;
}

inline int max(int a, int b) { return (a > b) ? a : b; }
inline int min(int a, int b) { return (a < b) ? a : b; }




void byteConversion(uint8_t *dataIn, size_t dataInBits, uint8_t *dataOut, size_t dataOutBits, size_t numValues) {
    memset(dataOut, 0, (numValues*dataOutBits-1)/8+1);
    size_t dataOutIndex = 0;
    size_t dataInIndex = 0;
    int dataOutStartbit = 0;
    int dataInStartbit = 0;
    int dataInEndbit = (int)dataInBits;
    int dataOutEndbit = (int)dataOutBits;
    for (size_t i = 0; i < numValues; i++) {
        /*
        printf("\n");
        printf("\n");
        printf("Value %i\n", i);*/
        while ((dataInStartbit<dataInEndbit) | (dataOutStartbit<dataOutEndbit)) {


            uint8_t data = readByte(dataIn[dataInIndex], dataInStartbit, dataInEndbit);
            dataOut[dataOutIndex] = writeByte(dataOut[dataOutIndex], data, dataOutStartbit, dataOutEndbit);

            /*
            printf("dataInStartbit %d\n", dataInStartbit);
            printf("dataInEndbit %d\n", dataInEndbit);
            printf("dataOutStartbit %d\n", dataOutStartbit);
            printf("dataOutEndbit %d\n", dataOutEndbit);
            printf("dataInIndex %d\n", dataInIndex);
            printf("dataOutIndex %d\n", dataOutIndex);
            printf("data");
            print_binary_uint8(data);
            printf("dataOut[dataOutIndex]");
            print_binary_uint8(dataOut[dataOutIndex]);
            */
            int valuesRead = min(dataInEndbit-dataInStartbit, 8-dataInStartbit % 8);
            int valuesWritten = min(dataOutEndbit-dataOutStartbit, 8-dataOutStartbit % 8);
            int minValue = min(valuesRead, valuesWritten);

            /*
            printf("valuesRead %d\n", valuesRead);
            printf("valuesWritten %d\n", valuesWritten);
            printf("minValue %d\n", minValue);*/

            uint8_t deltaIn = minValue;
            uint8_t deltaOut = minValue;
            if (dataInStartbit == dataInEndbit) {
                dataOutStartbit += valuesWritten;
                deltaOut = valuesWritten;

            } else {
                dataOutStartbit += minValue;
            }
            if (dataOutStartbit == dataOutEndbit) {
                dataInStartbit += valuesRead;
                deltaIn = valuesRead;
            }else {
                dataInStartbit += minValue;
            }

            if (dataInStartbit/8>(dataInStartbit-deltaIn)/8) {
                dataInIndex += 1;
            }
            if (dataOutStartbit/8>(dataOutStartbit-deltaOut)/8) {
                dataOutIndex += 1;
            }
            //printf("\n");

        }
        dataInStartbit = dataInEndbit % 8;
        dataInEndbit = dataInStartbit + dataInBits;
        dataOutStartbit = dataOutEndbit % 8;
        dataOutEndbit = dataOutStartbit + dataOutBits;
    }
}


void getTensorFromParameter(parameter_t *parameter, tensor_t *tensor, size_t *orderOfDimensions) {
    setOrderOfDimsForNewTensor(parameter->numberOfDimensions, orderOfDimensions);
    tensor->data = parameter->data;
    tensor->quantization = parameter->dataQuantization;
    tensor->sparsityBitmask = parameter->sparsityBitmask;
    tensor->dimensions = parameter->dimensions;
    tensor->numberOfDimensions = parameter->numberOfDimensions;
    tensor->orderOfDimensions = orderOfDimensions;
}

void getGradTensorFromParameter(parameter_t *parameter, tensor_t *tensor, size_t *orderOfDimensions) {
    setOrderOfDimsForNewTensor(parameter->numberOfDimensions, orderOfDimensions);
    tensor->data = parameter->grad;
    tensor->quantization = parameter->gradQuantization;
    tensor->sparsityBitmask = parameter->sparsityBitmask;
    tensor->dimensions = parameter->dimensions;
    tensor->numberOfDimensions = parameter->numberOfDimensions;
    tensor->orderOfDimensions = orderOfDimensions;
}

void initTensor(tensor_t *tensor, uint8_t *data, quantization_t *quantization,
                     uint8_t *sparsityBitmask,
                     size_t numberOfDims, size_t *dims, size_t *orderOfDimensions) {
    tensor->data = data;
    tensor->quantization = quantization;
    tensor->sparsityBitmask = sparsityBitmask;
    tensor->numberOfDimensions = numberOfDims;
    tensor->dimensions = dims;
    tensor->orderOfDimensions = orderOfDimensions;
}

void initParameter(parameter_t *parameter, uint8_t *data, quantization_t *dataQuantization,
                           uint8_t *sparsityBitmask, uint8_t *grad,
                           quantization_t *gradQuantization, size_t numberOfDims, size_t *dims) {
    parameter->data = data;
    parameter->dataQuantization = dataQuantization;
    parameter->sparsityBitmask = sparsityBitmask;
    parameter->grad = grad;
    parameter->gradQuantization = gradQuantization;
    parameter->numberOfDimensions = numberOfDims;
    parameter->dimensions = dims;
}

void zeroTensorData(tensor_t *tensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(tensor);
    size_t bytesPerElement = calcBytesPerElement(tensor->quantization);
    memset(tensor->data, 0, numberOfElements * bytesPerElement);
}

void transposeTensor(tensor_t *tensor, size_t dim0Index, size_t dim1Index) {
    if(tensor->numberOfDimensions < 2) {
        printf("Error in transpose: number of dims < 2\n");
        return;
    }
    size_t temp = tensor->orderOfDimensions[dim0Index];
    tensor->orderOfDimensions[dim0Index] = tensor->orderOfDimensions[dim1Index];
    tensor->orderOfDimensions[dim1Index] = temp;
}

void copyDimsAndSparsityToTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    outputTensor->numberOfDimensions = inputTensor->numberOfDimensions;
    memcpy(outputTensor->dimensions, inputTensor->dimensions, sizeof(size_t) * outputTensor->numberOfDimensions);
    if(inputTensor->sparsityBitmask) {
        memcpy(outputTensor->sparsityBitmask,
    inputTensor->sparsityBitmask,
    (calcNumberOfElementsByDims(outputTensor->numberOfDimensions, outputTensor->dimensions)-1)/8+1);
    }
    memcpy(outputTensor->orderOfDimensions, inputTensor->orderOfDimensions, sizeof(size_t) * outputTensor->numberOfDimensions);
}

void convertFloatTensorToInt32Tensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    float inputData[numberOfElements];
    int32_t outputData[numberOfElements];
    readBytesAsFloatArray(numberOfElements, inputTensor->data, inputData);
    zeroTensorData(outputTensor);
    for (size_t i = 0; i < numberOfElements; i++) {
        outputData[i] = (int32_t)inputData[i];
    }
    writeInt32ArrayToByteArray(numberOfElements, outputData, outputTensor->data);
    copyDimsAndSparsityToTensor(inputTensor, outputTensor);
}

void convertInt32TensorToFloatTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    int32_t inputData[numberOfElements];
    float outputData[numberOfElements];
    readBytesAsInt32Array(numberOfElements, inputTensor->data, inputData);
    zeroTensorData(outputTensor);
    for (size_t i = 0; i < numberOfElements; i++) {
        outputData[i] = (float)inputData[i];
    }
    writeFloatArrayToByteArray(numberOfElements, outputData, outputTensor->data);
    copyDimsAndSparsityToTensor(inputTensor, outputTensor);
}


void convertFloatTensorToLinearTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    size_t inputBytesPerElement = calcBytesPerElement(inputTensor->quantization);
    float min = findMinFloat(inputTensor->data, numberOfElements, inputBytesPerElement);
    float max = findMaxFloat(inputTensor->data, numberOfElements, inputBytesPerElement);
    linearQConfig_t *linearQConfig = outputTensor->quantization->qConfig;
    float qMax = pow(2, linearQConfig->qBits);
    float scale = (max - min) / qMax;
    int16_t zeroPoint = (int16_t)roundByMode(min / scale, linearQConfig->roundingMode);
    int32_t outputElements[numberOfElements];

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        float inputElement = readBytesAsFloat(&inputTensor->data[elementIndex*sizeof(float)]);

        outputElements[elementIndex] = roundByMode(clamp(inputElement / scale - (float)zeroPoint, 0.f, qMax-1), linearQConfig->roundingMode);
    }

    linearQConfig->scale = scale;
    linearQConfig->zeroPoint = zeroPoint;
    uint8_t outputElement[numberOfElements*sizeof(int32_t)];
    writeInt32ArrayToByteArray(numberOfElements, outputElements, outputElement);

    byteConversion(outputElement, 32, outputTensor->data, linearQConfig->qBits, numberOfElements);
    copyDimsAndSparsityToTensor(inputTensor, outputTensor);
}

void convertInt32TensorToLinearTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    size_t inputBytesPerElement = calcBytesPerElement(inputTensor->quantization);
    int32_t min = findMinInt32(inputTensor->data, numberOfElements, inputBytesPerElement);
    int32_t max = findMaxInt32(inputTensor->data, numberOfElements, inputBytesPerElement);
    linearQConfig_t *linearQConfig = outputTensor->quantization->qConfig;
    float qMax = pow(2, linearQConfig->qBits);
    float scale = (float)(max - min) / qMax;
    int16_t zeroPoint = (int16_t)roundByMode((float)min / scale, linearQConfig->roundingMode);
    int32_t outputElements[numberOfElements];

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        int32_t inputElement = readBytesAsInt32(&inputTensor->data[elementIndex*sizeof(int32_t)]);

        outputElements[elementIndex] = roundByMode(clamp((float)inputElement / scale - (float)zeroPoint, 0.f, qMax-1), linearQConfig->roundingMode);
    }
    linearQConfig->scale = scale;
    linearQConfig->zeroPoint = zeroPoint;
    uint8_t outputElement[numberOfElements*sizeof(int32_t)];
    writeInt32ArrayToByteArray(numberOfElements, outputElements, outputElement);

    byteConversion(outputElement, 32, outputTensor->data, linearQConfig->qBits, numberOfElements);
    copyDimsAndSparsityToTensor(inputTensor, outputTensor);
}

void convertLinearTensorToInt32Tensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    linearQConfig_t *linearQConfig = inputTensor->quantization->qConfig;
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);

    int16_t zeroPoint = linearQConfig->zeroPoint;
    uint8_t dataOut[numberOfElements*sizeof(int32_t)];
    memset(dataOut, 0, numberOfElements*sizeof(int32_t));
    byteConversion(inputTensor->data, linearQConfig->qBits, dataOut, 32, numberOfElements);
    int32_t outputElements[numberOfElements];
    readBytesAsInt32Array(numberOfElements, dataOut, outputElements);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        outputElements[elementIndex] = outputElements[elementIndex] + zeroPoint;
    }
    writeInt32ArrayToByteArray(numberOfElements, outputElements, outputTensor->data);
    copyDimsAndSparsityToTensor(inputTensor, outputTensor);
}

void convertLinearTensorToFloatTensor(tensor_t *inputTensor, tensor_t *outputTensor) {

    zeroTensorData(outputTensor);
    linearQConfig_t *linearQConfig = inputTensor->quantization->qConfig;
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    int16_t zeroPoint = linearQConfig->zeroPoint;
    uint8_t dataOut[numberOfElements*sizeof(int32_t)];

    memset(dataOut, 0, numberOfElements*sizeof(int32_t));

    byteConversion(inputTensor->data, linearQConfig->qBits, dataOut, 32, numberOfElements);

    int32_t intElements[numberOfElements];
    float outputElements[numberOfElements];
    readBytesAsInt32Array(numberOfElements, dataOut, intElements);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        outputElements[elementIndex] = ((float)intElements[elementIndex] + (float)zeroPoint)*linearQConfig->scale;
    }

    writeFloatArrayToByteArray(numberOfElements, outputElements, outputTensor->data);

    copyDimsAndSparsityToTensor(inputTensor, outputTensor);

}

conversionFunction_t conversionMatrix[3][3] = {
    [INT32] = {NULL, convertInt32TensorToFloatTensor, convertInt32TensorToLinearTensor},
    [FLOAT32] = {convertFloatTensorToInt32Tensor, NULL, convertFloatTensorToLinearTensor},
    [LINEAR] = {convertLinearTensorToInt32Tensor, convertLinearTensorToFloatTensor, NULL}
};


void convertTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    qtype_t inputDType = inputTensor->quantization->type;
    qtype_t outputDType = outputTensor->quantization->type;
    conversionFunction_t conversionFn = conversionMatrix[inputDType][outputDType];
    conversionFn(inputTensor, outputTensor);
}


// should not be needed anymore

float interpretLinearAsFloat(linearQConfig_t *linearQConfig, uint8_t linearRaw) {
    float scale = linearQConfig->scale;
    uint16_t zeroPoint = linearQConfig->zeroPoint;
    float linearElement = scale * (float)(linearRaw - zeroPoint);
    return linearElement;
}

float readTensorElementAsFloatByByteIndex(tensor_t *inputTensor, size_t elementByteIndex) {
    qtype_t type = inputTensor->quantization->type;
    float f;

    if (type == LINEAR) {
        linearQConfig_t *linearQConfig = inputTensor->quantization->qConfig;
        float scale = linearQConfig->scale;
        int16_t zeroPoint = linearQConfig->zeroPoint;
        uint8_t linearElementRaw = inputTensor->data[elementByteIndex];
        f = scale * (float)(linearElementRaw - zeroPoint);
        return f;
    }

    f = readBytesAsFloat(&inputTensor->data[elementByteIndex]);
    return f;
}

float readTensorElementAsInt32(tensor_t *inputTensor, size_t elementByteIndex) {
    qtype_t type = inputTensor->quantization->type;
    int32_t f;

    if (type == LINEAR) {
        linearQConfig_t *linearQConfig = inputTensor->quantization->qConfig;
        float scale = linearQConfig->scale;
        int16_t zeroPoint = linearQConfig->zeroPoint;
        uint8_t linearElementRaw = inputTensor->data[elementByteIndex];
        f = scale * (float)(linearElementRaw - zeroPoint);
        return f;
    }

    f = readBytesAsFloat(&inputTensor->data[elementByteIndex]);
    return f;
}

void writeFloatElementToTensor(tensor_t *tensor, size_t byteIndex, float value) {
    qtype_t type = tensor->quantization->type;

    switch(type) {
    case INT32:
        int32_t intValue = (int32_t)roundf(value);
        memcpy(&tensor->data[byteIndex], &intValue, sizeof(int32_t));
    case FLOAT32:
        memcpy(&tensor->data[byteIndex], &value, sizeof(float));
    case LINEAR:
        linearQConfig_t *linearQConfig = tensor->quantization->qConfig;
        float scale = linearQConfig->scale;
        int16_t zeroPoint = linearQConfig->zeroPoint;

    }

}

tensor_t buildTensorForConversion(uint8_t *data, quantization_t *q, tensor_t *originalTensor) {
    tensor_t output = {
        .data = data,
        .dimensions = originalTensor->dimensions,
        .quantization = q,
        .sparsityBitmask = originalTensor->sparsityBitmask,
        .numberOfDimensions = originalTensor->numberOfDimensions,
        .orderOfDimensions = originalTensor->orderOfDimensions
    };
    return output;
}

