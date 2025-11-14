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
    case ASYM:
        asymQConfig_t *asymQConfig = quantization->qConfig;
        uint32_t qBits = asymQConfig->qBits;
        return ceil((float)qBits / (float)8);
    default:
        return 0;
    }
}

size_t calcBitsPerElement(quantization_t *quantization) {
    switch (quantization->type) {
    case INT32:
        return sizeof(int32_t) * 8;
    case FLOAT32:
        return sizeof(float) * 8;
    case ASYM:
        asymQConfig_t *asymQConfig = quantization->qConfig;
        return asymQConfig->qBits;
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
    return bitsPerTensor / 8;
}

void setOrderOfDimsForNewTensor(size_t numberOfDimensions, size_t *orderOfDimensions) {
    for (size_t i = 0; i < numberOfDimensions; i++) {
        orderOfDimensions[i] = i;
    }
}

void print_binary_uint8(uint8_t x) {
    /* Show the most‑significant bit first */
    printf("Byte ");
    for (int i = 7; i >= 0; --i) {
        putchar((x >> i) & 1 ? '1' : '0');
    }
    putchar('\n'); /* newline for convenience */
}

uint32_t getBitmask(uint32_t startbit, uint32_t endbit) {
    uint32_t endbitInternal = endbit - (startbit / 8) * 8;
    uint32_t startbitInternal = startbit - (startbit / 8) * 8;
    uint32_t counter = 0;
    uint32_t value = 1;
    for (size_t i = 0; i < 8; i++) {
        if ((i >= startbitInternal) & (endbitInternal > i)) {
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
    intermediate >>= startbit - (startbit / 8) * 8;
    return intermediate;
}

uint8_t writeByte(uint8_t existingData, uint8_t data, uint8_t startbit, uint8_t endbit) {
    uint8_t startbitInternal = startbit - (startbit / 8) * 8;
    uint8_t endbitInternal = endbit - (startbit / 8) * 8;
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

inline int max(int a, int b) {
    return (a > b) ? a : b;
}

inline int min(int a, int b) {
    return (a < b) ? a : b;
}


void byteConversion(uint8_t *dataIn, size_t dataInBits, uint8_t *dataOut, size_t dataOutBits,
                    size_t numValues) {
    memset(dataOut, 0, (numValues * dataOutBits - 1) / 8 + 1);
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
        while ((dataInStartbit < dataInEndbit) | (dataOutStartbit < dataOutEndbit)) {

            uint8_t data = readByte(dataIn[dataInIndex], dataInStartbit, dataInEndbit);
            dataOut[dataOutIndex] = writeByte(dataOut[dataOutIndex], data, dataOutStartbit,
                                              dataOutEndbit);

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
            int valuesRead = min(dataInEndbit - dataInStartbit, 8 - dataInStartbit % 8);
            int valuesWritten = min(dataOutEndbit - dataOutStartbit, 8 - dataOutStartbit % 8);
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
            } else {
                dataInStartbit += minValue;
            }

            if (dataInStartbit / 8 > (dataInStartbit - deltaIn) / 8) {
                dataInIndex += 1;
            }
            if (dataOutStartbit / 8 > (dataOutStartbit - deltaOut) / 8) {
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

void getGradTensorFromParameter(parameter_t *parameter, tensor_t *tensor,
                                size_t *orderOfDimensions) {
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


void transposeTensor(tensor_t *tensor, size_t dim0Index, size_t dim1Index) {
    if (tensor->numberOfDimensions < 2) {
        printf("Error in transpose: number of dims < 2\n");
        return;
    }
    size_t temp = tensor->orderOfDimensions[dim0Index];
    tensor->orderOfDimensions[dim0Index] = tensor->orderOfDimensions[dim1Index];
    tensor->orderOfDimensions[dim1Index] = temp;
}


void setTensorValuesForConversion(uint8_t *data, quantization_t *q, tensor_t *originalTensor,
                                  tensor_t *outputTensor) {
    outputTensor->data = data;
    outputTensor->dimensions = originalTensor->dimensions;
    outputTensor->quantization = q;
    outputTensor->sparsityBitmask = originalTensor->sparsityBitmask;
    outputTensor->numberOfDimensions = originalTensor->numberOfDimensions;
    outputTensor->orderOfDimensions = originalTensor->orderOfDimensions;
}

void setTensorValues(tensor_t *tensor, uint8_t *data, size_t *dims, size_t numberOfDims,
                     size_t *orderOfDims,
                     quantization_t *quantization, uint8_t *sparsityBitmask) {
    tensor->data = data;
    tensor->dimensions = dims;
    tensor->quantization = quantization;
    tensor->sparsityBitmask = sparsityBitmask;
    tensor->numberOfDimensions = numberOfDims;
    tensor->orderOfDimensions = orderOfDims;
}

void printTensor(tensor_t *t) {
    quantization_t *q = t->quantization;
    printf("TENSOR BEGIN \n");
    size_t numValues = calcNumberOfElementsByTensor(t);
    int32_t data[numValues];
    switch (q->type) {
    case INT32:
        printf("INT32Q \n");
        readBytesAsInt32Array(numValues, t->data, data);
        for (size_t i = 0; i < numValues; i++) {
            printf("%i\n", data[i]);
        }
        break;
    case FLOAT32:
        printf("FLOAT32Q \n");
        for (size_t i = 0; i < numValues; i++) {
            size_t byteIndex = i * sizeof(float);
            float currentElement = readBytesAsFloat(&t->data[byteIndex]);
            printf("%f\n", currentElement);
        }
        break;
    case SYM_INT32:
        symInt32QConfig_t *symQC = q->qConfig;
        printf("SYM_INT32 \n");
        printf("scale=%f\n", symQC->scale);
        printf("Data \n");
        for (size_t i = 0; i < numValues; i++) {
            size_t byteIndex = i * sizeof(int32_t);
            int32_t currentElement = readBytesAsInt32(&t->data[byteIndex]);
            printf("%i\n", currentElement);
        }
        break;
    case ASYM:
        asymQConfig_t *lq = q->qConfig;
        printf("ASYM\n");
        printf("scale=%f\n", lq->scale);
        printf("offset=%i\n", lq->zeroPoint);
        printf("Data \n");
        for (size_t i = 0; i < numValues; i++) {
            printf("%i\n", t->data[i]);
        }
        break;
    default:
        printf("WTF");
    }

    printf("TENSOR END \n");
    printf("\n");
}

void initOrderOfDimensions(size_t *orderOfDims, size_t numberOfDims) {
    for (size_t i = 0; i < numberOfDims; i++) {
        orderOfDims[i] = i;
    }
}
