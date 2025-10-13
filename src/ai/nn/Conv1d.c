#define SOURCE_FILE "AI_NN_Conv1d"

#include "Conv1d.h"

#include <Common.h>
#include <stdlib.h>
#include <string.h>

size_t calcOutputSizeForInputSizeAndConv1dConfig(size_t inputSize, conv1dConfig_t *conv1dConfig) {

    if (conv1dConfig->padding->type == SAME) {
        return inputSize / conv1dConfig->inputChannels *conv1dConfig->outputChannels;
    }

    size_t padding = conv1dConfig->padding->size;
    size_t dilation = conv1dConfig->dilation;
    size_t kernelSize = conv1dConfig->kernelSize;
    size_t stride = conv1dConfig->stride;
    size_t inputChannels = conv1dConfig->inputChannels;
    size_t inputSizePerChannel = inputSize / inputChannels;
    size_t outputChannels = conv1dConfig->outputChannels;

    size_t effectiveKernelSize = (kernelSize - 1) * dilation + 1;
    size_t outputSizePerChannel = (inputSizePerChannel + 2 * padding - effectiveKernelSize) / stride
                                  + 1;

    return outputSizePerChannel * outputChannels;
}

qTensor_t *initConv1dOutputTensor(conv1dConfig_t *conv1dConfig, qTensor_t *inputQTensor) {
    qTensor_t *outputTensor = calloc(1, sizeof(qTensor_t));

    if (inputQTensor->numberOfDimensions != 2) {
        printf("Error: Conv1d expects input tensor with 2 dimensions (C, L)\n");
        return NULL;
    }

    size_t outputChannels = conv1dConfig->outputChannels;
    size_t inputLength = inputQTensor->dimensions[1];

    size_t kernelSize = conv1dConfig->kernelSize;
    size_t stride = conv1dConfig->stride;
    size_t dilation = conv1dConfig->dilation;
    size_t padding = conv1dConfig->padding->size;

    size_t effectiveKernel = (kernelSize - 1) * dilation + 1;
    size_t outputLength = (inputLength + 2 * padding - effectiveKernel) / stride + 1;

    if (conv1dConfig->padding->type == SAME) {
        outputLength = (inputLength + stride - 1) / stride;
    }

    outputTensor->numberOfDimensions = 2;
    outputTensor->dimensions = calloc(2, sizeof(size_t));
    outputTensor->dimensions[0] = outputChannels;
    outputTensor->dimensions[1] = outputLength;

    size_t totalElements = outputChannels * outputLength;

    outputTensor->data = calloc(totalElements, sizeof(float));

    return outputTensor;
}


conv1dConfig_t *initConv1dConfigWithWeightAndBias(parameterQTensor_t *weightQTensor,
                                                  parameterQTensor_t *biasQTensor,
                                                  size_t inputChannels,
                                                  size_t outputChannels,
                                                  size_t kernelSize,
                                                  size_t stride,
                                                  size_t dilation,
                                                  paddingType_t paddingType,
                                                  size_t paddingSize) {

    conv1dConfig_t *conv1dConfig = calloc(1, sizeof(conv1dConfig_t));

    conv1dConfig->inputChannels = inputChannels;
    conv1dConfig->outputChannels = outputChannels;
    conv1dConfig->kernelSize = kernelSize;
    conv1dConfig->weight = weightQTensor;
    conv1dConfig->bias = biasQTensor;

    conv1dConfig->stride = stride;
    conv1dConfig->dilation = dilation;

    conv1dConfig->padding = calloc(1, sizeof(padding_t));
    conv1dConfig->padding->type = paddingType;
    conv1dConfig->padding->size = paddingSize;

    if (biasQTensor->dataTensor->data) {
        conv1dConfig->useBias = true;
    } else {
        conv1dConfig->bias = NULL;
        conv1dConfig->useBias = false;
    }

    return conv1dConfig;
}


conv1dConfig_t *initConv1dConfigWithKernels(parameterQTensor_t *kernels,
                                            parameterQTensor_t *bias,
                                            size_t inputChannels,
                                            size_t outputChannels,
                                            size_t kernelSize,
                                            size_t stride,
                                            size_t dilation,
                                            paddingType_t paddingType,
                                            size_t paddingSize) {

    conv1dConfig_t *config = calloc(1, sizeof(*config));
    config->inputChannels = inputChannels;
    config->outputChannels = outputChannels;
    config->kernelSize = kernelSize;
    config->stride = stride;
    config->dilation = dilation;

    config->padding = calloc(1, sizeof(*config->padding));
    config->padding->type = paddingType;
    config->padding->size = paddingSize;

    config->weight = kernels;
    // TODO FIX
    config->weight->gradTensor = calloc(outputChannels * inputChannels * kernelSize,
                                  sizeof(float));

    if (bias->dataTensor->data) {
        config->useBias = true;
    }

    return config;
}


conv1dConfig_t *initConv1dConfig(size_t inputChannels, size_t outputChannels, size_t kernelSize,
                                 size_t stride, size_t dilation, bool useBias,
                                 paddingType_t paddingType, size_t paddingSize) {
    conv1dConfig_t *conv1dConfig = calloc(1, sizeof(conv1dConfig_t));
    conv1dConfig->inputChannels = inputChannels;
    conv1dConfig->outputChannels = outputChannels;
    conv1dConfig->kernelSize = kernelSize;
    conv1dConfig->stride = stride;
    conv1dConfig->dilation = dilation;
    conv1dConfig->padding->type = paddingType;
    conv1dConfig->padding->size = paddingSize;
    conv1dConfig->useBias = useBias;

    return conv1dConfig;
}

qTensor_t *conv1dForward(void *config, qTensor_t *inputQTensor) {
    conv1dConfig_t *conv1dConfig = config;

    size_t inChannels = conv1dConfig->inputChannels;
    size_t outChannels = conv1dConfig->outputChannels;
    size_t kernel = conv1dConfig->kernelSize;
    size_t stride = conv1dConfig->stride;
    size_t dilation = conv1dConfig->dilation;
    size_t inSizePerChannel = calcTotalNumberOfElementsByTensor(inputQTensor) / inChannels;

    // SAME padding berechnen
    size_t outSizePerChannel;
    size_t padLeft = 0, padRight = 0;
    if (conv1dConfig->padding->type == SAME) {
        outSizePerChannel = (inSizePerChannel + stride - 1) / stride; // ceil
        size_t padNeeded = (outSizePerChannel - 1) * stride
                           + dilation * (kernel - 1) + 1
                           - inSizePerChannel;
        if ((int)padNeeded < 0)
            padNeeded = 0;
        padLeft = padNeeded / 2;
        padRight = padNeeded - padLeft;
    } else {
        // VALID
        outSizePerChannel = (inSizePerChannel + 2 * conv1dConfig->padding->size
                             - dilation * (kernel - 1) - 1) / stride + 1;
        padLeft = conv1dConfig->padding->size;
        padRight = conv1dConfig->padding->size;
    }

    qTensor_t *output = initConv1dOutputTensor(conv1dConfig, inputQTensor);

    for (size_t oc = 0; oc < outChannels; ++oc) {
        for (size_t i = 0; i < outSizePerChannel; ++i) {
            float sum = 0.f;
            for (size_t ic = 0; ic < inChannels; ++ic) {
                for (size_t k = 0; k < kernel; ++k) {
                    int inIndex = (int)i * (int)stride
                                  + (int)k * (int)dilation
                                  - (int)padLeft;
                    if (inIndex < 0 || inIndex >= (int)inSizePerChannel)
                        continue;

                    size_t inIdx = ic * inSizePerChannel + (size_t)inIndex;
                    size_t wIdx = oc * inChannels * kernel + ic * kernel + k;

                    sum += inputQTensor->data[inIdx] * conv1dConfig->weight->dataTensor->data[wIdx];
                }
            }
            if (conv1dConfig->useBias) {
                sum += conv1dConfig->bias->dataTensor->data[oc];
            }
            output->data[oc * outSizePerChannel + i] = sum;
        }
    }
    return output;
}


qTensor_t *conv1dBackward(void *config, qTensor_t *gradQTensor, qTensor_t *inputQTensor) {
    conv1dConfig_t *conv1dConfig = config;

    size_t inChannels = conv1dConfig->inputChannels;
    size_t outChannels = conv1dConfig->outputChannels;
    size_t kernel = conv1dConfig->kernelSize;
    size_t stride = conv1dConfig->stride;
    size_t dilation = conv1dConfig->dilation;
    size_t inputSize = calcTotalNumberOfElementsByTensor(inputQTensor);
    size_t inSizePerChannel = inputSize / inChannels;

    // SAME padding berechnen
    size_t outSizePerChannel;
    size_t padLeft = 0, padRight = 0;
    if (conv1dConfig->padding->type == SAME) {
        outSizePerChannel = (inSizePerChannel + stride - 1) / stride;
        size_t padNeeded = (outSizePerChannel - 1) * stride
                           + dilation * (kernel - 1) + 1
                           - inSizePerChannel;
        if ((int)padNeeded < 0)
            padNeeded = 0;
        padLeft = padNeeded / 2;
        padRight = padNeeded - padLeft;
    } else {
        // VALID
        outSizePerChannel = (inSizePerChannel + 2 * conv1dConfig->padding->size
                             - dilation * (kernel - 1) - 1) / stride + 1;
        padLeft = conv1dConfig->padding->size;
        padRight = conv1dConfig->padding->size;
    }

    qTensor_t *gradInput = calloc(1, sizeof(qTensor_t));
    gradInput->data = calloc(inputSize, sizeof(float));
    gradInput->dimensions = calloc(inputQTensor->numberOfDimensions, sizeof(size_t));
    gradInput->numberOfDimensions = inputQTensor->numberOfDimensions;
    memcpy(gradInput->dimensions, inputQTensor->dimensions, inputQTensor->numberOfDimensions * sizeof(float));

    // bias gradient
    if (conv1dConfig->useBias) {
        for (size_t oc = 0; oc < outChannels; ++oc) {
            float sumB = 0.f;
            for (size_t i = 0; i < outSizePerChannel; ++i) {
                sumB += gradQTensor->data[oc * outSizePerChannel + i];
            }
            conv1dConfig->bias->gradTensor->data[oc] += sumB;
        }
    }

    // weight gradient & gradInput
    for (size_t oc = 0; oc < outChannels; ++oc) {
        for (size_t ic = 0; ic < inChannels; ++ic) {
            for (size_t k = 0; k < kernel; ++k) {
                for (size_t i = 0; i < outSizePerChannel; ++i) {
                    int inIndex = (int)i * (int)stride
                                  + (int)k * (int)dilation
                                  - (int)padLeft;
                    if (inIndex < 0 || inIndex >= (int)inSizePerChannel)
                        continue;

                    float g = gradQTensor->data[oc * outSizePerChannel + i];

                    size_t inIdx = ic * inSizePerChannel + (size_t)inIndex;
                    size_t wIdx = oc * inChannels * kernel + ic * kernel + k;

                    conv1dConfig->weight->gradTensor->data[wIdx] += g * inputQTensor->data[inIdx];
                    gradInput->data[inIdx] += g * conv1dConfig->weight->dataTensor->data[wIdx];
                }
            }
        }
    }

    return gradInput;
}

layerForward_t *initConv1dLayerForward(parameterQTensor_t *weightQTensor, parameterQTensor_t *biasQTensor,
                                       size_t inputChannels,
                                       size_t outputChannels, size_t kernelSize, size_t stride,
                                       size_t dilation, paddingType_t paddingType,
                                       size_t paddingSize) {
    layerForward_t *layerForward = calloc(1, sizeof(layerForward_t));
    layerForward->config = initConv1dConfigWithWeightAndBias(
        weightQTensor, biasQTensor, inputChannels, outputChannels, kernelSize, stride, dilation,
        paddingType,
        paddingSize);
    layerForward->type = CONV1D;

    return layerForward;
}

layerForwardBackward_t *initConv1dLayerForwardBackward(parameterQTensor_t *weightQTensor, parameterQTensor_t *biasQTensor,
                                                       size_t inputChannels,
                                                       size_t outputChannels, size_t kernelSize,
                                                       size_t stride,
                                                       size_t dilation, paddingType_t paddingType,
                                                       size_t paddingSize) {
    layerForwardBackward_t *layerForwardBackward = calloc(1, sizeof(layerForwardBackward_t));
    layerForwardBackward->config = initConv1dConfigWithWeightAndBias(
        weightQTensor, biasQTensor, inputChannels, outputChannels, kernelSize, stride, dilation,
        paddingType,
        paddingSize);
    layerForwardBackward->type = CONV1D;

    return layerForwardBackward;
}
