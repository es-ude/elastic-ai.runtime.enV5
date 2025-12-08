#include "CrossEntropy.h"
#include "TensorConversion.h"
#include "Log.h"

float crossEntropyForwardFloat(tensor_t *softmaxOutput, tensor_t *distribution) {
    size_t numberOfValues = calcNumberOfElementsByTensor(softmaxOutput);

    float *softmaxOutputFloat = (float *)softmaxOutput->data;
    float *distributionFloat = (float *)distribution->data;

    float loss = 0.f;
    for (size_t i = 0; i < numberOfValues; i++) {
        loss += distributionFloat[i] * -logFloat(softmaxOutputFloat[i]);
    }

    return loss;
}

static void crossEntropySoftmaxBackwardFloat(tensor_t *softmaxOutput, tensor_t *distribution, tensor_t *loss) {
    size_t totalInputSize = calcNumberOfElementsByTensor(softmaxOutput);

    float *softmaxOutputFloat = (float *)softmaxOutput->data;
    float *distributionFloat = (float *)distribution->data;
    float *lossFloat = (float *)loss->data;

    for (size_t i = 0; i < totalInputSize; i++) {
        lossFloat[i] = softmaxOutputFloat[i] - distributionFloat[i];
    }
}

static void crossEntropySoftmaxBackwardAsym(tensor_t *softmaxOutput, tensor_t *distribution, tensor_t *loss) {
    size_t inputSize = calcNumberOfElementsByTensor(softmaxOutput);

    tensor_t softmaxOutputFloat;
    quantization_t softmaxOutputFloatQ;
    initFloat32Quantization(&softmaxOutputFloatQ);
    float softmaxOutputFloatData[inputSize];
    setTensorValuesForConversion(softmaxOutputFloatData, &softmaxOutputFloatQ, softmaxOutput, &softmaxOutputFloat);
    convertTensor(softmaxOutput, &softmaxOutputFloat);

    tensor_t distributionFloat;
    quantization_t distributionFloatQ;
    initFloat32Quantization(&distributionFloatQ);
    float distributionFloatData[inputSize];
    setTensorValuesForConversion(distributionFloatData, &distributionFloatQ, distribution, &distributionFloat);
    convertTensor(distribution, &distributionFloat);

    tensor_t lossFloat;
    quantization_t lossFloatQ;
    initFloat32Quantization(&lossFloatQ);
    float lossFloatData[inputSize];
    setTensorValuesForConversion(lossFloatData, &lossFloatQ, loss, &lossFloat);
    convertTensor(loss, &lossFloat);


    float *softmaxOutputFloatArr = (float *)softmaxOutputFloat.data;
    float *distributionFloatArr = (float *)distributionFloat.data;
    float *lossFloatArr = (float *)lossFloat.data;

    for (size_t i = 0; i < inputSize; i++) {
        lossFloatArr[i] = softmaxOutputFloatArr[i] - distributionFloatArr[i];
    }

    convertTensor(&lossFloat, loss);
}

// IMPORTANT: This implementation already takes the softmax backward into account
void crossEntropySoftmaxBackward(tensor_t *softmaxOutput, tensor_t *distribution, tensor_t *loss) {
    switch(softmaxOutput->quantization->type) {
    case FLOAT32:
        crossEntropySoftmaxBackwardFloat(softmaxOutput, distribution, loss);
        break;
    case ASYM:
        crossEntropySoftmaxBackwardAsym(softmaxOutput, distribution, loss);
    default:
        break;
    }
}