#include "Matmul.h"
#include "Mul.h"
#include "Tensor.h"
#include "Arithmetic.h"

#include <DTypes.h>
#include <stdio.h>
#include <string.h>

size_t matmulInstructionCounter = 0;

void matmulInt32Tensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    if (aTensor->numberOfDimensions > 2 || bTensor->numberOfDimensions > 2) {
        printf("Error: Matmul only supports up to 2D Tensors\n");
        return;
    }

    size_t aNumberOfDims = aTensor->numberOfDimensions;
    size_t *aDims = aTensor->dimensions;

    size_t bNumberOfDims = bTensor->numberOfDimensions;
    size_t *bDims = bTensor->dimensions;

    size_t aRows, aColumns;
    if (aNumberOfDims < 2) {
        aRows = 1;
        aColumns = aTensor->dimensions[0];
    } else {
        aRows = aTensor->dimensions[0];
        aColumns = aTensor->dimensions[1];
    }

    size_t bRows = bTensor->dimensions[0];
    size_t bColumns = 0;
    if (bNumberOfDims < 2) {
        bColumns = 1;
    } else {
        bColumns = bTensor->dimensions[1];
    }

    size_t resultCounter = 0;

    if (aColumns != bRows) {
        printf("Error Matmul: Rows dont match Columns\n");
        return;
    }

    for (size_t rowIndex = 0; rowIndex < aRows; rowIndex++) {

        for (size_t columnIndex = 0; columnIndex < bColumns; columnIndex++) {
            int32_t result = 0;
            for (size_t i = 0; i < aColumns; i++) {
                size_t aByteIndex = 0;
                if (aNumberOfDims == 1) {
                    aByteIndex = i * sizeof(int32_t);
                } else {
                    size_t aIndices[] = {rowIndex, i};
                    size_t aValueIndex = calcElementIndexByIndices(
                        aNumberOfDims, aDims, aIndices, aTensor->orderOfDimensions);
                    aByteIndex = aValueIndex * sizeof(int32_t);
                }

                int32_t aValue = readBytesAsInt32(&aTensor->data[aByteIndex]);

                size_t bByteIndex = 0;
                if (bNumberOfDims == 1) {
                    bByteIndex = i * sizeof(int32_t);
                } else {
                    size_t bIndices[] = {i, columnIndex};
                    size_t bValueIndex = calcElementIndexByIndices(
                        bNumberOfDims, bDims, bIndices, bTensor->orderOfDimensions);
                    bByteIndex = bValueIndex * sizeof(int32_t);
                }

                int32_t bValue = readBytesAsInt32(&bTensor->data[bByteIndex]);

                result += mulInt32s(aValue, bValue);

            }
            //printf("result: %i\n", result);

            size_t outputByteIndex = resultCounter * sizeof(int32_t);
            memcpy(&outputTensor->data[outputByteIndex], &result, sizeof(int32_t));
            resultCounter++;
        }
    }
    ++matmulInstructionCounter;
}


void matmulFloatTensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    if (aTensor->numberOfDimensions > 2 || bTensor->numberOfDimensions > 2) {
        printf("Error: Matmul only supports up to 2D Tensors\n");
        return;
    }

    size_t aNumberOfDims = aTensor->numberOfDimensions;
    size_t *aDims = aTensor->dimensions;

    size_t bNumberOfDims = bTensor->numberOfDimensions;
    size_t *bDims = bTensor->dimensions;

    size_t aRows, aColumns = 0;
    if (aNumberOfDims < 2) {
        aRows = 1;
        aColumns = getDimensionsByIndex(aTensor, 0);
    } else {
        aRows = getDimensionsByIndex(aTensor, 0);
        aColumns = getDimensionsByIndex(aTensor, 1);
    }

    size_t bRows, bColumns = 0;
    if (bNumberOfDims < 2) {
        bRows = getDimensionsByIndex(bTensor, 0);
        bColumns = 1;
    } else {
        bRows = getDimensionsByIndex(bTensor, 0);
        bColumns = getDimensionsByIndex(bTensor, 1);

    }

    size_t resultCounter = 0;

    //printf("aCol: %lu, bRows: %lu\n", aColumns, bRows);

    if (aColumns != bRows) {
        printf("Error Matmul: Rows dont match Columns\n");
        return;
    }

    for (size_t rowIndex = 0; rowIndex < aRows; rowIndex++) {

        for (size_t columnIndex = 0; columnIndex < bColumns; columnIndex++) {
            float result = 0;
            for (size_t i = 0; i < aColumns; i++) {
                size_t aByteIndex = 0;

                if (aNumberOfDims == 1) {
                    aByteIndex = i * sizeof(float);
                } else {
                    size_t aIndices[] = {rowIndex, i};
                    size_t aValueIndex = calcElementIndexByIndices(
                        aNumberOfDims, aDims, aIndices, aTensor->orderOfDimensions);
                    aByteIndex = aValueIndex * sizeof(float);
                }

                float aValue = readBytesAsFloat(&aTensor->data[aByteIndex]);

                size_t bByteIndex = 0;
                if (bNumberOfDims == 1) {
                    bByteIndex = i * sizeof(float);
                } else {
                    size_t bIndices[] = {i, columnIndex};

                    size_t bValueIndex = calcElementIndexByIndices(
                        bNumberOfDims, bDims, bIndices, bTensor->orderOfDimensions);
                    bByteIndex = bValueIndex * sizeof(float);
                }

                float bValue = readBytesAsFloat(&bTensor->data[bByteIndex]);
                result += mulFloats(aValue, bValue);

            }

            size_t outputByteIndex = resultCounter * sizeof(float);
            memcpy(&outputTensor->data[outputByteIndex], &result, sizeof(float));
            resultCounter++;
        }
    }
    ++matmulInstructionCounter;
}

void matMulTensors(tensor_t *a, tensor_t *b, tensor_t *output) {
    qtype_t aType = a->quantization->type;
    qtype_t bType = b->quantization->type;

    if (aType != bType) {
        printf("Error in Matmul: Tensors have mismatched qtypes");
        return;
    }

    switch (aType) {
    case FLOAT32:
        matmulFloatTensors(a, b, output);
        break;
    case INT32:
        matmulInt32Tensors(a, b, output);
        break;
    default:
        printf("Error in Matmul: Tensor qtype not supported");
        break;
    }
}

size_t getMatmulInstructionCounter() {
    return matmulInstructionCounter;
}
