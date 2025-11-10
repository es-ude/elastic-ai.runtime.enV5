#include "TensorConversion.h"
#include "Tensor.h"
#include "DTypes.h"
#include "math.h"
#include "MinMax.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

void zeroTensorData(tensor_t *tensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(tensor);
    size_t bytesPerElement = calcBytesPerElement(tensor->quantization);
    memset(tensor->data, 0, numberOfElements * bytesPerElement);
}

void copyDimsAndSparsityToTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    outputTensor->numberOfDimensions = inputTensor->numberOfDimensions;
    memcpy(outputTensor->dimensions, inputTensor->dimensions,
           sizeof(size_t) * outputTensor->numberOfDimensions);
    if (inputTensor->sparsityBitmask) {
        memcpy(outputTensor->sparsityBitmask,
               inputTensor->sparsityBitmask,
               (calcNumberOfElementsByDims(outputTensor->numberOfDimensions,
                                           outputTensor->dimensions) - 1) / 8 + 1);
    }
    memcpy(outputTensor->orderOfDimensions, inputTensor->orderOfDimensions,
           sizeof(size_t) * outputTensor->numberOfDimensions);
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

void convertInt32TensorToSymInt32Tensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);

    symInt32QConfig_t *outputSymInt32QConfig = outputTensor->quantization->qConfig;
    outputSymInt32QConfig->scale = 1;

    memcpy(outputTensor->data, inputTensor->data, numberOfElements * sizeof(int32_t));
}

// conversion from int to asym should not be needed/used
void convertInt32TensorToAsymTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    int32_t min = findMinInt32(inputTensor->data, numberOfElements);
    int32_t max = findMaxInt32(inputTensor->data, numberOfElements);
    asymQConfig_t *linearQConfig = outputTensor->quantization->qConfig;
    int32_t qMax = pow(2, linearQConfig->qBits) - 1;

    float scale = (float)(max - min) / (float)qMax;
    int16_t zeroPoint = (int16_t)roundByMode((float)min / scale, linearQConfig->roundingMode);

    int32_t outputElements[numberOfElements];
    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        int32_t inputElement = readBytesAsInt32(&inputTensor->data[elementIndex * sizeof(int32_t)]);

        outputElements[elementIndex] = roundByMode(
            clamp((float)inputElement / scale - (float)zeroPoint, 0.f, qMax - 1),
            linearQConfig->roundingMode);
    }
    linearQConfig->scale = scale;
    linearQConfig->zeroPoint = zeroPoint;
    uint8_t outputElement[numberOfElements * sizeof(int32_t)];
    writeInt32ArrayToByteArray(numberOfElements, outputElements, outputElement);

    byteConversion(outputElement, 32, outputTensor->data, linearQConfig->qBits, numberOfElements);
    copyDimsAndSparsityToTensor(inputTensor, outputTensor);
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

void convertFloatTensorToSymInt32Tensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);

    float input[numberOfElements];
    readBytesAsFloatArray(numberOfElements, inputTensor->data, input);
    int32_t inputAsInt32[numberOfElements];

    for (size_t i = 0; i < numberOfElements; i++) {
        inputAsInt32[i] = (int32_t)input[i];
    }

    symInt32QConfig_t *outputSymInt32QConfig = outputTensor->quantization->qConfig;
    outputSymInt32QConfig->scale = 1;

    memcpy(outputTensor->data, inputAsInt32, numberOfElements * sizeof(int32_t));
}

// I DON'T HAVE TO IMPLEMENT SYM CONVERSIONS!
void convertFloatTensorToSymTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);

    float min = findMinFloat(inputTensor->data, numberOfElements);
    float max = findMaxFloat(inputTensor->data, numberOfElements);

    symQConfig_t *outputSymQConfig = outputTensor->quantization->qConfig;
    float qMax = pow(2, outputSymQConfig->qBits);

    float scale = (min - max) / qMax;
    outputSymQConfig->scale = scale;

    float inputs[numberOfElements];
    readBytesAsFloatArray(numberOfElements, inputTensor->data, inputs);

    size_t bytesPerOutputElement = calcBytesPerElement(outputTensor->quantization);
    uint8_t outputs[numberOfElements * bytesPerOutputElement];

    for (size_t i = 0; i < numberOfElements; i++) {
        outputs[i] = roundByMode(clamp(inputs[i] / scale, 0.f, qMax - 1),
                                 outputSymQConfig->roundingMode);
    }

    memcpy(outputTensor->data, outputs, numberOfElements * bytesPerOutputElement);
}

// conversion from float to asym should not be needed/used
void convertFloatTensorToAsymTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    float min = findMinFloat(inputTensor->data, numberOfElements);
    float max = findMaxFloat(inputTensor->data, numberOfElements);
    asymQConfig_t *asymQConfig = outputTensor->quantization->qConfig;
    float qMax = pow(2, asymQConfig->qBits);
    float scale = (max - min) / qMax;

    int16_t zeroPoint = (int16_t)roundByMode(min / scale, asymQConfig->roundingMode);
    // todo change int32 to uint8_t with numberOfElements * bytesPerElements
    int32_t outputElements[numberOfElements];

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        float inputElement = readBytesAsFloat(&inputTensor->data[elementIndex * sizeof(float)]);

        outputElements[elementIndex] = roundByMode(
            clamp(inputElement / scale - (float)zeroPoint, 0.f, qMax - 1),
            asymQConfig->roundingMode);
    }

    asymQConfig->scale = scale;
    asymQConfig->zeroPoint = zeroPoint;
    uint8_t outputElement[numberOfElements * sizeof(int32_t)];
    writeInt32ArrayToByteArray(numberOfElements, outputElements, outputElement);

    byteConversion(outputElement, 32, outputTensor->data, asymQConfig->qBits, numberOfElements);
    copyDimsAndSparsityToTensor(inputTensor, outputTensor);
}

// Important: Scale is ignored!
void extractInt32TensorFromSymInt32Tensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    size_t bytesPerElement = sizeof(int32_t);

    int32_t inputAsInt32[numberOfElements];
    readBytesAsInt32Array(numberOfElements, inputTensor->data, inputAsInt32);

    memcpy(outputTensor->data, inputAsInt32, numberOfElements * bytesPerElement);
}

void convertSymInt32TensorToFloat32Tensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfValues = calcNumberOfElementsByTensor(inputTensor);
    size_t bytesPerOutputElement = sizeof(float);

    int32_t inputAsInt32[numberOfValues];
    readBytesAsInt32Array(numberOfValues, inputTensor->data, inputAsInt32);

    float output[numberOfValues];

    symInt32QConfig_t *inputSymInt32QConfig = inputTensor->quantization->qConfig;
    float scale = inputSymInt32QConfig->scale;

    for (size_t i = 0; i < numberOfValues; i++) {
        output[i] = (float)inputAsInt32[i] * scale;
    }
    memcpy(outputTensor->data, output, numberOfValues * bytesPerOutputElement);
}

// could maybe be improved by doing int to asym and then multiply scales
void convertSymInt32TensorToAsymTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfValues = calcNumberOfElementsByTensor(inputTensor);
    size_t bytesPerOutputValue = calcBytesPerElement(outputTensor->quantization);

    symInt32QConfig_t *inputSymInt32QConfig = inputTensor->quantization->qConfig;
    asymQConfig_t *outputAsymQConfig = outputTensor->quantization->qConfig;

    float inputScale = inputSymInt32QConfig->scale;

    int32_t inputAsInt32[numberOfValues];
    readBytesAsInt32Array(numberOfValues, inputTensor->data, inputAsInt32);

    float inputAsFloat[numberOfValues];
    for (size_t i = 0; i < numberOfValues; i++) {
        inputAsFloat[i] = inputScale * (float)inputAsInt32[i];
    }

    float min = findMinFloat(inputAsFloat, numberOfValues);
    float max = findMaxFloat(inputAsFloat, numberOfValues);
    int32_t qMax = (1 << outputAsymQConfig->qBits) - 1;

    float outputScale = (max - min) / (float)qMax;
    outputAsymQConfig->scale = outputScale;

    int16_t zeroPoint = (int16_t)roundByMode(min / outputScale, outputAsymQConfig->roundingMode);
    outputAsymQConfig->zeroPoint = zeroPoint;

    int32_t outputInt[numberOfValues];
    for (size_t i = 0; i < numberOfValues; i++) {
        outputInt[i] = roundByMode(
            clamp(inputAsFloat[i] / outputScale - (float)zeroPoint, 0.f, qMax),
            outputAsymQConfig->roundingMode);
    }

    size_t outputBitsPerElement = calcBitsPerElement(outputTensor->quantization);
    size_t totalNumberOfRequiredBits = numberOfValues * outputBitsPerElement;
    size_t numberOfRequiredBytes = ceil((double)totalNumberOfRequiredBits / (double)8);

    uint8_t output[numberOfRequiredBytes];
    byteConversion(outputInt, 32, output, outputAsymQConfig->qBits, numberOfValues);

    memcpy(outputTensor->data, output, numberOfRequiredBytes);
}


void convertAsymTensorToInt32Tensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    asymQConfig_t *asymQConfig = inputTensor->quantization->qConfig;
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);

    int16_t zeroPoint = asymQConfig->zeroPoint;
    uint8_t dataOut[numberOfElements * sizeof(int32_t)];
    memset(dataOut, 0, numberOfElements * sizeof(int32_t));
    byteConversion(inputTensor->data, asymQConfig->qBits, dataOut, 32, numberOfElements);
    int32_t outputElements[numberOfElements];
    readBytesAsInt32Array(numberOfElements, dataOut, outputElements);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        outputElements[elementIndex] = outputElements[elementIndex] + zeroPoint;
    }
    writeInt32ArrayToByteArray(numberOfElements, outputElements, outputTensor->data);
    copyDimsAndSparsityToTensor(inputTensor, outputTensor);
}

void convertAsymTensorToFloatTensor(tensor_t *inputTensor, tensor_t *outputTensor) {

    zeroTensorData(outputTensor);
    asymQConfig_t *linearQConfig = inputTensor->quantization->qConfig;
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    int16_t zeroPoint = linearQConfig->zeroPoint;
    uint8_t dataOut[numberOfElements * sizeof(int32_t)];

    memset(dataOut, 0, numberOfElements * sizeof(int32_t));

    byteConversion(inputTensor->data, linearQConfig->qBits, dataOut, 32, numberOfElements);

    int32_t intElements[numberOfElements];
    float outputElements[numberOfElements];
    readBytesAsInt32Array(numberOfElements, dataOut, intElements);

    for (size_t elementIndex = 0; elementIndex < numberOfElements; elementIndex++) {
        outputElements[elementIndex] = ((float)intElements[elementIndex] + (float)zeroPoint) *
                                       linearQConfig->scale;
    }

    writeFloatArrayToByteArray(numberOfElements, outputElements, outputTensor->data);

    copyDimsAndSparsityToTensor(inputTensor, outputTensor);

}

void convertAsymTensorToSymInt32Tensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    size_t numberOfValues = calcNumberOfElementsByTensor(inputTensor);
    size_t bitsPerInputElement = calcBitsPerElement(inputTensor->quantization);
    size_t bytesPerOutputElement = sizeof(int32_t);

    asymQConfig_t *inputAsymQConfig = inputTensor->quantization->qConfig;
    symInt32QConfig_t *outputSymInt32QConfig = outputTensor->quantization->qConfig;

    int16_t zeroPoint = inputAsymQConfig->zeroPoint;
    int32_t inputAsInt32[numberOfValues];

    byteConversion(inputTensor->data, bitsPerInputElement, inputAsInt32, 32, numberOfValues);

    for (size_t i = 0; i < numberOfValues; i++) {
        inputAsInt32[i] += zeroPoint;
    }

    memcpy(outputTensor->data, inputAsInt32, numberOfValues * bytesPerOutputElement);
    outputSymInt32QConfig->scale = inputAsymQConfig->scale;
}


char *quantTypeToString(qtype_t t) {
    switch (t) {
    case INT32:
        return "INT32";
    case FLOAT32:
        return "FLOAT32";
    case SYM_INT32:
        return "SYMINT32";
    case SYM:
        return "SYM";
    case ASYM:
        return "ASYM";
    default:
        return "UNKNOWN";
    }
}

void unsupportedConversionTypes(tensor_t *inputTensor, tensor_t *outputTensor) {
    qtype_t inputQType = inputTensor->quantization->type;
    qtype_t outputQType = outputTensor->quantization->type;

    printf("Error in tensor conversion: Conversion from %s to %s is not supported\n",
           quantTypeToString(inputQType), quantTypeToString(outputQType));
}

conversionFunction_t conversionMatrix[5][5] = {
    [INT32] = {
        [INT32] = NULL,
        [FLOAT32] = convertInt32TensorToFloatTensor,
        [SYM_INT32] = convertInt32TensorToSymInt32Tensor,
        [SYM] = unsupportedConversionTypes,
        [ASYM] = convertInt32TensorToAsymTensor
    },
    [FLOAT32] = {
        [INT32] = convertFloatTensorToInt32Tensor,
        [FLOAT32] = NULL,
        [SYM_INT32] = convertFloatTensorToSymInt32Tensor,
        [SYM] = unsupportedConversionTypes,
        [ASYM] = convertFloatTensorToAsymTensor
    },
    [SYM_INT32] = {
        [INT32] = extractInt32TensorFromSymInt32Tensor,
        [FLOAT32] = convertSymInt32TensorToFloat32Tensor,
        [SYM_INT32] = NULL,
        [SYM] = unsupportedConversionTypes,
        [ASYM] = convertSymInt32TensorToAsymTensor
    },
    [SYM] = {
        [INT32] = unsupportedConversionTypes,
        [FLOAT32] = unsupportedConversionTypes,
        [SYM_INT32] = unsupportedConversionTypes,
        [SYM] = NULL,
        [ASYM] = unsupportedConversionTypes
    },
    [ASYM] = {
        [INT32] = convertAsymTensorToInt32Tensor,
        [FLOAT32] = convertAsymTensorToFloatTensor,
        [SYM_INT32] = convertAsymTensorToSymInt32Tensor,
        [SYM] = unsupportedConversionTypes,
        [ASYM] = NULL
    }
};


void convertTensor(tensor_t *inputTensor, tensor_t *outputTensor) {
    qtype_t inputDType = inputTensor->quantization->type;
    qtype_t outputDType = outputTensor->quantization->type;
    size_t numberOfElements = calcNumberOfElementsByTensor(inputTensor);
    size_t bytesPerElement = calcBytesPerElement(inputTensor->quantization);

    if (inputDType == outputDType) {
        memcpy(outputTensor->data, inputTensor->data, numberOfElements * bytesPerElement);
    } else {
        conversionFunction_t conversionFn = conversionMatrix[inputDType][outputDType];
        conversionFn(inputTensor, outputTensor);
    }
}
