#include "Matmul.h"
#include "Mul.h"
#include "Tensor.h"
#include "Arithmetic.h"
#include "DTypes.h"
#include "TensorConversion.h"

#include <stdio.h>

#ifdef TRACK_INSTRUCTIONS
#define MATMUL_FUNC_INT matmulIntTensorsWithInstructionCounter
#define MATMUL_FUNC_FLOAT matmulFloatTensorsWithInstructionCounter
#define MATMUL_FUNC_SYM_INT32 matmulSymIntTensorsWithInstructionCounter
#else
#define MATMUL_FUNC_INT matmulIntTensors
#define MATMUL_FUNC_FLOAT matmulFloatTensors
#define MATMUL_FUNC_SYM_INT32 matmulSymIntTensors
#endif

size_t matmulInstructionCounter = 0;

void matmulIntTensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    if (aTensor->shape.numberOfDimensions > 2 || bTensor->shape.numberOfDimensions > 2) {
        printf("Error: Matmul only supports up to 2D Tensors\n");
        return;
    }

    size_t aNumberOfDims = aTensor->shape.numberOfDimensions;
    size_t *aDims = aTensor->shape.dimensions;

    size_t bNumberOfDims = bTensor->shape.numberOfDimensions;
    size_t *bDims = bTensor->shape.dimensions;

    size_t aRows, aColumns;
    if (aNumberOfDims < 2) {
        aRows = 1;
        aColumns = getDimensionsByIndex(aTensor, 0);
    } else {
        aRows = getDimensionsByIndex(aTensor, 0);
        aColumns = getDimensionsByIndex(aTensor, 1);
    }

    size_t bRows = getDimensionsByIndex(bTensor, 0);
    size_t bColumns = 0;
    if (bNumberOfDims < 2) {
        bColumns = 1;
    } else {
        bColumns = getDimensionsByIndex(bTensor, 1);
    }

    size_t resultCounter = 0;

    if (aColumns != bRows) {
        printf("Error Matmul: Rows dont match Columns\n");
        printf("bRows: %lu, bCols: %lu\n", bRows, bColumns);
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
                        aNumberOfDims, aDims, aIndices, aTensor->shape.orderOfDimensions);
                    aByteIndex = aValueIndex * sizeof(int32_t);
                }

                int32_t aValue = readBytesAsInt32(&aTensor->data[aByteIndex]);

                size_t bByteIndex = 0;
                if (bNumberOfDims == 1) {
                    bByteIndex = i * sizeof(int32_t);
                } else {
                    size_t bIndices[] = {i, columnIndex};
                    size_t bValueIndex = calcElementIndexByIndices(
                        bNumberOfDims, bDims, bIndices, bTensor->shape.orderOfDimensions);
                    bByteIndex = bValueIndex * sizeof(int32_t);
                }

                int32_t bValue = readBytesAsInt32(&bTensor->data[bByteIndex]);

                result += mulInt32s(aValue, bValue);

            }
            //printf("result: %i\n", result);

            size_t outputByteIndex = resultCounter * sizeof(int32_t);

            writeInt32ToByteArray(result, &outputTensor->data[outputByteIndex]);
            resultCounter++;
        }
    }
}

void matmulIntTensorsWithInstructionCounter(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    if (aTensor->shape.numberOfDimensions > 2 || bTensor->shape.numberOfDimensions > 2) {
        printf("Error: Matmul only supports up to 2D Tensors\n");
        return;
    }

    size_t aNumberOfDims = aTensor->shape.numberOfDimensions;
    size_t *aDims = aTensor->shape.dimensions;

    size_t bNumberOfDims = bTensor->shape.numberOfDimensions;
    size_t *bDims = bTensor->shape.dimensions;

    size_t aRows, aColumns;
    if (aNumberOfDims < 2) {
        aRows = 1;
        aColumns = getDimensionsByIndex(aTensor, 0);
    } else {
        aRows = getDimensionsByIndex(aTensor, 0);
        aColumns = getDimensionsByIndex(aTensor, 1);
    }

    size_t bRows = getDimensionsByIndex(bTensor, 0);
    size_t bColumns = 0;
    if (bNumberOfDims < 2) {
        bColumns = 1;
    } else {
        bColumns = getDimensionsByIndex(bTensor, 1);
    }

    size_t resultCounter = 0;

    if (aColumns != bRows) {
        printf("Error Matmul: Rows dont match Columns\n");
        printf("bRows: %lu, bCols: %lu\n", bRows, bColumns);
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
                        aNumberOfDims, aDims, aIndices, aTensor->shape.orderOfDimensions);
                    aByteIndex = aValueIndex * sizeof(int32_t);
                }

                int32_t aValue = readBytesAsInt32(&aTensor->data[aByteIndex]);

                size_t bByteIndex = 0;
                if (bNumberOfDims == 1) {
                    bByteIndex = i * sizeof(int32_t);
                } else {
                    size_t bIndices[] = {i, columnIndex};
                    size_t bValueIndex = calcElementIndexByIndices(
                        bNumberOfDims, bDims, bIndices, bTensor->shape.orderOfDimensions);
                    bByteIndex = bValueIndex * sizeof(int32_t);
                }

                int32_t bValue = readBytesAsInt32(&bTensor->data[bByteIndex]);

                result += mulInt32s(aValue, bValue);

            }
            //printf("result: %i\n", result);

            size_t outputByteIndex = resultCounter * sizeof(int32_t);

            writeInt32ToByteArray(result, &outputTensor->data[outputByteIndex]);
            resultCounter++;
        }
    }
    ++matmulInstructionCounter;
}

void matmulInt32Tensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    MATMUL_FUNC_INT(aTensor, bTensor, outputTensor);
}

void matmulFloatTensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    if (aTensor->shape.numberOfDimensions > 2 || bTensor->shape.numberOfDimensions > 2) {
        printf("Error: Matmul only supports up to 2D Tensors\n");
        return;
    }

    size_t aNumberOfDims = aTensor->shape.numberOfDimensions;
    size_t *aDims = aTensor->shape.dimensions;

    size_t bNumberOfDims = bTensor->shape.numberOfDimensions;
    size_t *bDims = bTensor->shape.dimensions;

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
                        aNumberOfDims, aDims, aIndices, aTensor->shape.orderOfDimensions);
                    aByteIndex = aValueIndex * sizeof(float);
                }

                float aValue = readBytesAsFloat(&aTensor->data[aByteIndex]);

                size_t bByteIndex = 0;
                if (bNumberOfDims == 1) {
                    bByteIndex = i * sizeof(float);
                } else {
                    size_t bIndices[] = {i, columnIndex};

                    size_t bValueIndex = calcElementIndexByIndices(
                        bNumberOfDims, bDims, bIndices, bTensor->shape.orderOfDimensions);
                    bByteIndex = bValueIndex * sizeof(float);
                }

                float bValue = readBytesAsFloat(&bTensor->data[bByteIndex]);
                result += mulFloat32s(aValue, bValue);

            }

            size_t outputByteIndex = resultCounter * sizeof(float);

            writeFloatToByteArray(result, &outputTensor->data[outputByteIndex]);
            resultCounter++;
        }
    }
}

void matmulFloatTensorsWithInstructionCounter(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    if (aTensor->shape.numberOfDimensions > 2 || bTensor->shape.numberOfDimensions > 2) {
        printf("Error: Matmul only supports up to 2D Tensors\n");
        return;
    }

    size_t aNumberOfDims = aTensor->shape.numberOfDimensions;
    size_t *aDims = aTensor->shape.dimensions;

    size_t bNumberOfDims = bTensor->shape.numberOfDimensions;
    size_t *bDims = bTensor->shape.dimensions;

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
                        aNumberOfDims, aDims, aIndices, aTensor->shape.orderOfDimensions);
                    aByteIndex = aValueIndex * sizeof(float);
                }

                float aValue = readBytesAsFloat(&aTensor->data[aByteIndex]);

                size_t bByteIndex = 0;
                if (bNumberOfDims == 1) {
                    bByteIndex = i * sizeof(float);
                } else {
                    size_t bIndices[] = {i, columnIndex};

                    size_t bValueIndex = calcElementIndexByIndices(
                        bNumberOfDims, bDims, bIndices, bTensor->shape.orderOfDimensions);
                    bByteIndex = bValueIndex * sizeof(float);
                }

                float bValue = readBytesAsFloat(&bTensor->data[bByteIndex]);
                result += mulFloat32s(aValue, bValue);

            }

            size_t outputByteIndex = resultCounter * sizeof(float);

            writeFloatToByteArray(result, &outputTensor->data[outputByteIndex]);
            resultCounter++;
        }
    }
    ++matmulInstructionCounter;
}

void matmulFloat32Tensors(tensor_t *aTensor, tensor_t *bTensor, tensor_t *outputTensor) {
    MATMUL_FUNC_FLOAT(aTensor, bTensor, outputTensor);
}

void matmulSymIntTensors(tensor_t* aTensor, tensor_t* bTensor, tensor_t* outputTensor) {

    quantization_t int32Q;
    initInt32Quantization(&int32Q);

    size_t aNumberOfValues = calcNumberOfElementsByTensor(aTensor);
    int32_t aIntData[aNumberOfValues];
    tensor_t aInt;
    setTensorValuesForConversion(aIntData, &int32Q, aTensor, &aInt);
    convertTensor(aTensor, &aInt);

    size_t bNumberOfValues = calcNumberOfElementsByTensor(bTensor);
    int32_t bIntData[bNumberOfValues];
    tensor_t bInt;
    setTensorValuesForConversion(bIntData, &int32Q, bTensor, &bInt);
    convertTensor(bTensor, &bInt);

    size_t outputNumberOfValues = calcNumberOfElementsByTensor(outputTensor);
    int32_t outputIntData[outputNumberOfValues];
    tensor_t outputInt;
    setTensorValuesForConversion(outputIntData, &int32Q, outputTensor, &outputInt);

    matmulInt32Tensors(&aInt, &bInt, &outputInt);

    convertTensor(&outputInt, outputTensor);

    symInt32QConfig_t *aSymInt32QConfig = aTensor->quantization->qConfig;
    symInt32QConfig_t *bSymInt32QConfig = bTensor->quantization->qConfig;
    symInt32QConfig_t *outputSymInt32QConfig = outputTensor->quantization->qConfig;

    outputSymInt32QConfig->scale = aSymInt32QConfig->scale * bSymInt32QConfig->scale;
}

void matmulSymIntTensorsWithInstructionCounter(tensor_t* aTensor, tensor_t* bTensor, tensor_t* outputTensor) {

    quantization_t int32Q;
    initInt32Quantization(&int32Q);

    size_t aNumberOfValues = calcNumberOfElementsByTensor(aTensor);
    int32_t aIntData[aNumberOfValues];
    tensor_t aInt;
    setTensorValuesForConversion(aIntData, &int32Q, aTensor, &aInt);
    convertTensor(aTensor, &aInt);

    size_t bNumberOfValues = calcNumberOfElementsByTensor(bTensor);
    int32_t bIntData[bNumberOfValues];
    tensor_t bInt;
    setTensorValuesForConversion(bIntData, &int32Q, bTensor, &bInt);
    convertTensor(bTensor, &bInt);

    size_t outputNumberOfValues = calcNumberOfElementsByTensor(outputTensor);
    int32_t outputIntData[outputNumberOfValues];
    tensor_t outputInt;
    setTensorValuesForConversion(outputIntData, &int32Q, outputTensor, &outputInt);

    matmulInt32Tensors(&aInt, &bInt, &outputInt);

    convertTensor(&outputInt, outputTensor);

    symInt32QConfig_t *aSymInt32QConfig = aTensor->quantization->qConfig;
    symInt32QConfig_t *bSymInt32QConfig = bTensor->quantization->qConfig;
    symInt32QConfig_t *outputSymInt32QConfig = outputTensor->quantization->qConfig;

    outputSymInt32QConfig->scale = aSymInt32QConfig->scale * bSymInt32QConfig->scale;

    ++matmulInstructionCounter;
}

void matmulSymInt32Tensors(tensor_t* aTensor, tensor_t* bTensor, tensor_t* outputTensor) {
    MATMUL_FUNC_SYM_INT32(aTensor, bTensor, outputTensor);
}

size_t getMatmulInstructionCounter() {
    return matmulInstructionCounter;
}