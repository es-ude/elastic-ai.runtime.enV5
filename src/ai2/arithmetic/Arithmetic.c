//
// Created by Leo Buron on 20.10.25.
//

#include "Arithmetic.h"

#include <DTypes.h>
#include <stdio.h>
#include <string.h>

size_t getDimensionsByIndex(tensor_t *tensor, size_t index) {
    size_t numberOfDims = tensor->numberOfDimensions;
    for (size_t i = 0; i < numberOfDims; i++) {
        if (tensor->orderOfDimensions[i] == index) {
            return tensor->dimensions[i];
        }
    }
}

void orderDims(tensor_t *tensor, size_t *orderedDims) {
    for (size_t i = 0; i < tensor->numberOfDimensions; i++) {
        orderedDims[i] = getDimensionsByIndex(tensor, i);
    }
}

bool doDimensionsMatch(tensor_t *a, tensor_t *b) {
    size_t aNumberOfDims = a->numberOfDimensions;
    size_t bNumberOfDims = b->numberOfDimensions;

    if (aNumberOfDims != bNumberOfDims) {
        return false;
    }

    size_t aOrderedDims[aNumberOfDims];
    size_t bOrderedDims[bNumberOfDims];

    orderDims(a, aOrderedDims);
    orderDims(b, bOrderedDims);

    for (size_t i = 0; i < aNumberOfDims; i++) {
        if (aOrderedDims[i] != bOrderedDims[i]) {
            return false;
        }
    }
    return true;
};

size_t calcTensorIndexByIndices(size_t numberOfDimensions, size_t *dimensions, size_t *indices) {
    size_t index = indices[numberOfDimensions - 1];
    size_t offset = dimensions[numberOfDimensions - 1];
    for (int i = (int)numberOfDimensions - 2; i >= 0; i--) {
        index += indices[i] * offset;
        offset *= dimensions[i];
    }
    return index;
}

void calcIndicesByRawIndex(size_t numberOfDims, size_t *dims, size_t rawIndex, size_t *indices) {
    size_t offset = 1;
    // equal to numberOfElements
    for (size_t i = 0; i < numberOfDims; i++) {
        offset *= dims[i];
    }
    size_t restIndex = rawIndex;
    for (size_t i = 0; i < numberOfDims; i++) {
        offset /= dims[i];
        indices[i] = restIndex / offset;
        restIndex -= indices[i] * offset;
    }
}

size_t calcElementIndexByIndices(size_t numberOfDims, size_t *dims, size_t *indices) {
    size_t offset = 1;
    // equal to numberOfElements
    for (size_t i = 0; i < numberOfDims; i++) {
        offset *= dims[i];
    }

    size_t outputIndex = 0;
    for (size_t i = 0; i < numberOfDims; i++) {
        offset /= dims[i];
        outputIndex += indices[i] * offset;
    }
    return outputIndex;
}


void int32PointWiseArithmetic(tensor_t *aTensor, tensor_t *bTensor,
                              int32ElementArithmeticFunc_t arithmeticFunc, tensor_t *outputTensor) {
    if (!doDimensionsMatch(aTensor, bTensor)) {
        printf("Error: Dimensions dont match\n");
        return;
    }

    size_t numberOfElements = calcNumberOfElementsByTensor(aTensor);
    size_t bytesPerElement = sizeof(int32_t);
    size_t numberOfDims = aTensor->numberOfDimensions;
    size_t *aDims = aTensor->dimensions;
    size_t *bDims = bTensor->dimensions;

    size_t aOrderedDims[numberOfDims];
    size_t bOrderedDims[numberOfDims];

    orderDims(aTensor, aOrderedDims);
    orderDims(bTensor, bOrderedDims);

    for (size_t i = 0; i < numberOfElements; i++) {
        size_t *aIndices[numberOfDims];
        calcIndicesByRawIndex(numberOfDims, aDims, i, aIndices);
        size_t aElementIndex = calcElementIndexByIndices(numberOfDims, aDims, aIndices);

        size_t *bIndices[numberOfDims];
        calcIndicesByRawIndex(numberOfDims, bDims, i, bIndices);
        size_t bElementIndex = calcElementIndexByIndices(numberOfDims, bDims, bIndices);

        size_t aByteIndex = aElementIndex * bytesPerElement;
        size_t bByteIndex = bElementIndex * bytesPerElement;

        int32_t aValue = readBytesAsInt32(&aTensor->data[aByteIndex]);
        int32_t bValue = readBytesAsInt32(&bTensor->data[bByteIndex]);

        int32_t result = arithmeticFunc(aValue, bValue);

        size_t outputByteIndex = i * bytesPerElement;
        memcpy(&outputTensor->data[outputByteIndex], &result, bytesPerElement);
    }
}

void int32PointWiseArithmeticInplace(tensor_t *aTensor, tensor_t *bTensor,
                                     int32ElementArithmeticFunc_t arithmeticFunc) {
    if (!doDimensionsMatch(aTensor, bTensor)) {
        printf("Error: Dimensions dont match\n");
        return;
    }

    size_t numberOfElements = calcNumberOfElementsByTensor(aTensor);
    size_t bytesPerElement = sizeof(int32_t);
    size_t numberOfDims = aTensor->numberOfDimensions;
    size_t *aDims = aTensor->dimensions;
    size_t *bDims = bTensor->dimensions;

    size_t aOrderedDims[numberOfDims];
    size_t bOrderedDims[numberOfDims];

    orderDims(aTensor, aOrderedDims);
    orderDims(bTensor, bOrderedDims);

    for (size_t i = 0; i < numberOfElements; i++) {
        size_t *aIndices[numberOfDims];
        calcIndicesByRawIndex(numberOfDims, aDims, i, aIndices);
        size_t aElementIndex = calcElementIndexByIndices(numberOfDims, aDims, aIndices);

        size_t *bIndices[numberOfDims];
        calcIndicesByRawIndex(numberOfDims, bDims, i, bIndices);
        size_t bElementIndex = calcElementIndexByIndices(numberOfDims, bDims, bIndices);

        size_t aByteIndex = aElementIndex * bytesPerElement;
        size_t bByteIndex = bElementIndex * bytesPerElement;

        int32_t aValue = readBytesAsInt32(&aTensor->data[aByteIndex]);
        int32_t bValue = readBytesAsInt32(&bTensor->data[bByteIndex]);

        int32_t result = arithmeticFunc(aValue, bValue);

        size_t outputByteIndex = i * bytesPerElement;
        memcpy(&aTensor->data[outputByteIndex], &result, bytesPerElement);
    }
}

void int32ElementWithTensorArithmeticInplace(tensor_t *tensor, int32_t x,
                                             int32ElementArithmeticFunc_t arithmeticFunc) {

    size_t numberOfElements = calcNumberOfElementsByTensor(tensor);
    size_t bytesPerElement = sizeof(int32_t);

    for (size_t i = 0; i < numberOfElements; i++) {
        size_t byteIndex = i * bytesPerElement;
        int32_t currentElement = readBytesAsInt32(&tensor->data[byteIndex]);
        int32_t result = arithmeticFunc(currentElement, x);

        memcpy(&tensor->data[byteIndex], &result, bytesPerElement);
    }
}

void int32ElementWithTensorArithmetic(tensor_t *tensor, int32_t x,
                                      int32ElementArithmeticFunc_t arithmeticFunc,
                                      tensor_t *outputTensor) {

    size_t numberOfElements = calcNumberOfElementsByTensor(tensor);
    size_t bytesPerElement = sizeof(int32_t);

    for (size_t i = 0; i < numberOfElements; i++) {
        size_t byteIndex = i * bytesPerElement;
        int32_t currentElement = readBytesAsInt32(&tensor->data[byteIndex]);
        int32_t result = arithmeticFunc(currentElement, x);

        memcpy(&outputTensor->data[byteIndex], &result, bytesPerElement);
    }
}


void floatPointWiseArithmetic(tensor_t *aTensor, tensor_t *bTensor,
                              floatElementArithmeticFunc_t arithmeticFunc,
                              tensor_t *outputTensor) {
    if (!doDimensionsMatch(aTensor, bTensor)) {
        printf("Error: Dimensions dont match\n");
        return;
    }

    size_t numberOfElements = calcNumberOfElementsByTensor(aTensor);
    size_t bytesPerElement = sizeof(float);
    size_t numberOfDims = aTensor->numberOfDimensions;
    size_t *aDims = aTensor->dimensions;
    size_t *bDims = bTensor->dimensions;

    size_t aOrderedDims[numberOfDims];
    size_t bOrderedDims[numberOfDims];

    orderDims(aTensor, aOrderedDims);
    orderDims(bTensor, bOrderedDims);

    for (size_t i = 0; i < numberOfElements; i++) {
        size_t *aIndices[numberOfDims];
        calcIndicesByRawIndex(numberOfDims, aDims, i, aIndices);
        size_t aElementIndex = calcElementIndexByIndices(numberOfDims, aDims, aIndices);

        size_t *bIndices[numberOfDims];
        calcIndicesByRawIndex(numberOfDims, bDims, i, bIndices);
        size_t bElementIndex = calcElementIndexByIndices(numberOfDims, bDims, bIndices);

        size_t aByteIndex = aElementIndex * bytesPerElement;
        size_t bByteIndex = bElementIndex * bytesPerElement;

        float aValue = readBytesAsFloat(&aTensor->data[aByteIndex]);
        float bValue = readBytesAsFloat(&bTensor->data[bByteIndex]);

        float result = arithmeticFunc(aValue, bValue);

        size_t outputByteIndex = i * bytesPerElement;
        memcpy(&outputTensor->data[outputByteIndex], &result, bytesPerElement);
    }
}

void floatPointWiseArithmeticInplace(tensor_t *aTensor, tensor_t *bTensor,
                                     floatElementArithmeticFunc_t arithmeticFunc) {
    if (!doDimensionsMatch(aTensor, bTensor)) {
        printf("Error: Dimensions dont match\n");
        return;
    }

    size_t numberOfElements = calcNumberOfElementsByTensor(aTensor);
    size_t bytesPerElement = sizeof(float);
    size_t numberOfDims = aTensor->numberOfDimensions;
    size_t *aDims = aTensor->dimensions;
    size_t *bDims = bTensor->dimensions;

    size_t aOrderedDims[numberOfDims];
    size_t bOrderedDims[numberOfDims];

    orderDims(aTensor, aOrderedDims);
    orderDims(bTensor, bOrderedDims);

    for (size_t i = 0; i < numberOfElements; i++) {
        size_t *aIndices[numberOfDims];
        calcIndicesByRawIndex(numberOfDims, aDims, i, aIndices);
        size_t aElementIndex = calcElementIndexByIndices(numberOfDims, aDims, aIndices);

        size_t *bIndices[numberOfDims];
        calcIndicesByRawIndex(numberOfDims, bDims, i, bIndices);
        size_t bElementIndex = calcElementIndexByIndices(numberOfDims, bDims, bIndices);

        size_t aByteIndex = aElementIndex * bytesPerElement;
        size_t bByteIndex = bElementIndex * bytesPerElement;

        float aValue = readBytesAsFloat(&aTensor->data[aByteIndex]);
        float bValue = readBytesAsFloat(&bTensor->data[bByteIndex]);

        float result = arithmeticFunc(aValue, bValue);

        size_t outputByteIndex = i * bytesPerElement;
        memcpy(&aTensor->data[outputByteIndex], &result, bytesPerElement);
    }
}

void floatElementWithTensorArithmetic(tensor_t *tensor, float x,
                                      floatElementArithmeticFunc_t arithmeticFunc,
                                      tensor_t *outputTensor) {

    size_t numberOfElements = calcNumberOfElementsByTensor(tensor);
    size_t bytesPerElement = sizeof(float);

    for (size_t i = 0; i < numberOfElements; i++) {
        size_t byteIndex = i * bytesPerElement;
        float currentValue = readBytesAsFloat(&tensor->data[byteIndex]);
        float result = arithmeticFunc(currentValue, x);
        memcpy(&outputTensor->data[byteIndex], &result, bytesPerElement);
    }
}

void floatElementWithTensorArithmeticInplace(tensor_t *tensor, float x,
                                             floatElementArithmeticFunc_t arithmeticFunc) {

    size_t numberOfElements = calcNumberOfElementsByTensor(tensor);
    size_t bytesPerElement = sizeof(float);

    for (size_t i = 0; i < numberOfElements; i++) {
        size_t byteIndex = i * bytesPerElement;
        float currentValue = readBytesAsFloat(&tensor->data[byteIndex]);
        float result = arithmeticFunc(currentValue, x);
        memcpy(&tensor->data[byteIndex], &result, bytesPerElement);
    }
}


