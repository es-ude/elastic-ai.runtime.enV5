#include "Softmax.h"
#include "TensorConversion.h"
#include <math.h>

#define EULER_APPROX = 2.71828

void initSoftmaxLayer(layer_t *softmaxLayer) {
    softmaxLayer->type = SOFTMAX;
}


static void softmaxForwardFloat(tensor_t *input, tensor_t *output) {
    size_t inputSize = calcNumberOfElementsByTensor(input);

    float *inputFloat = (float *)input->data;
    float *outputFloat = (float *)output->data;

    float sum = 0;
    for (size_t i = 0; i < inputSize; i++) {
        sum += expf(inputFloat[i]);
    }

    for (size_t i = 0; i < inputSize; i++) {
        outputFloat[i] = expf(inputFloat[i]) / sum;
    }
}

static void softmaxForwardAsym(tensor_t *input, tensor_t *output) {
    size_t inputSize = calcNumberOfElementsByTensor(input);

    tensor_t inputFloat;
    quantization_t inputFloatQ;
    initFloat32Quantization(&inputFloatQ);
    float inputFloatData[inputSize];
    setTensorValuesForConversion(inputFloatData, &inputFloatQ, input, &inputFloat);
    convertTensor(input, &inputFloat);

    tensor_t outputFloat;
    quantization_t outputFloatQ;
    initFloat32Quantization(&outputFloatQ);
    float outputFloatData[inputSize];
    setTensorValuesForConversion(outputFloatData, &outputFloatQ, output, &outputFloat);
    convertTensor(output, &outputFloat);

    float *inputFloatArr = (float *)inputFloat.data;
    float *outputFloatArr = (float *)outputFloat.data;

    float sum = 0;
    for (size_t i = 0; i < inputSize; i++) {
        sum += expf(inputFloatArr[i]);
    }

    for (size_t i = 0; i < inputSize; i++) {
        outputFloatArr[i] = expf(inputFloatArr[i]) / sum;
    }

    convertTensor(&outputFloat, output);
}

void softmaxForward(layer_t *softmaxLayer, tensor_t *input, tensor_t *output) {
    switch (softmaxLayer->inputQ->type) {
    case FLOAT32:
        softmaxForwardFloat(input, output);
        break;
    case ASYM:
        softmaxForwardAsym(input, output);
    default:
        break;
    }
}

static void softmaxBackwardFloat(tensor_t *input, tensor_t *loss, tensor_t *propLoss) {

    size_t inputSize = calcNumberOfElementsByTensor(input);

    float *inputFloat = (float *)input->data;
    float *lossFloat = (float *)loss->data;
    float *propLossFloat = (float *)propLoss->data;

    float jacobian[inputSize][inputSize];

    for (size_t i = 0; i < inputSize; i++) {
        for (size_t j = 0; j < inputSize; j++) {
            if (i == j) {
                jacobian[i][j] = inputFloat[i] * (1 - inputFloat[i]);
            } else {
                jacobian[i][j] = -inputFloat[i] * inputFloat[j];
            }
        }
    }

    for (size_t i = 0; i < inputSize; i++) {
        float sum = 0;
        for (size_t j = 0; j < inputSize; j++) {
            sum += jacobian[i][j] * lossFloat[j];
        }
        propLossFloat[i] = sum;
    }
}

static void softmaxBackwardAsym(tensor_t *input, tensor_t *loss, tensor_t *propLoss) {
    size_t inputSize = calcNumberOfElementsByTensor(input);

    tensor_t inputFloat;
    quantization_t inputFloatQ;
    initFloat32Quantization(&inputFloatQ);
    float inputFloatData[inputSize];
    setTensorValuesForConversion(inputFloatData, &inputFloatQ, input, &inputFloat);
    convertTensor(input, &inputFloat);

    tensor_t lossFloat;
    quantization_t lossFloatQ;
    initFloat32Quantization(&lossFloatQ);
    float lossFloatData[inputSize];
    setTensorValuesForConversion(lossFloatData, &lossFloatQ, loss, &lossFloat);
    convertTensor(loss, &lossFloat);

    tensor_t propLossFloat;
    quantization_t propLossFloatQ;
    initFloat32Quantization(&propLossFloatQ);
    float propLossFloatData[inputSize];
    setTensorValuesForConversion(propLossFloatData, &propLossFloatQ, propLoss, &propLossFloat);
    convertTensor(propLoss, &propLossFloat);

    float *inputFloatArr = (float *)inputFloat.data;
    float *lossFloatArr = (float *)lossFloat.data;
    float *propLossFloatArr = (float *)propLossFloat.data;

    float jacobian[inputSize][inputSize];

    for (size_t i = 0; i < inputSize; i++) {
        for (size_t j = 0; j < inputSize; j++) {
            if (i == j) {
                jacobian[i][j] = inputFloatArr[i] * (1 - inputFloatArr[i]);
            } else {
                jacobian[i][j] = -inputFloatArr[i] * inputFloatArr[j];
            }
        }
    }

    for (size_t i = 0; i < inputSize; i++) {
        float sum = 0;
        for (size_t j = 0; j < inputSize; j++) {
            sum += jacobian[i][j] * lossFloatArr[j];
        }
        propLossFloatArr[i] = sum;
    }

    convertTensor(&propLossFloat, propLoss);
}

void softmaxBackward(layer_t *softmaxLayer, tensor_t *input, tensor_t *loss, tensor_t *propLoss) {
    switch (softmaxLayer->inputQ->type) {
    case FLOAT32:
        softmaxBackwardFloat(input, loss, propLoss);
        break;
    case ASYM:
        softmaxBackwardAsym(input, loss, propLoss);
        break;
    default:
        break;
    }
}
