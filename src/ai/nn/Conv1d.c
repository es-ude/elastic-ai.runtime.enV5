#define SOURCE_FILE "AI_NN_Conv1d"

#include "Conv1d.h"

#include <Common.h>
#include <string.h>

size_t calcOutputSizeForInputSizeAndConv1dConfig(size_t inputSize, conv1dConfig_t *conv1dConfig) {

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

conv1dConfig_t *initConv1dConfigWithWeightAndBias(float *weight,
                                                  float *bias,
                                                  size_t inputChannels,
                                                  size_t outputChannels,
                                                  size_t kernelSize,
                                                  size_t stride,
                                                  size_t dilation,
                                                  paddingType_t paddingType,
                                                  size_t paddingSize) {

    conv1dConfig_t *conv1dConfig = malloc(sizeof(conv1dConfig_t));

    conv1dConfig->inputChannels = inputChannels;
    conv1dConfig->outputChannels = outputChannels;
    conv1dConfig->kernelSize = kernelSize;

    conv1dConfig->stride = stride;
    conv1dConfig->dilation = dilation;

    conv1dConfig->padding = malloc(sizeof(padding_t));
    conv1dConfig->padding->type = paddingType;
    conv1dConfig->padding->size = paddingSize;

    size_t weightSize = inputChannels * outputChannels * kernelSize;
    conv1dConfig->weight = malloc(weightSize * sizeof(float));
    conv1dConfig->weight->size = weightSize;
    conv1dConfig->weight->p = malloc(weightSize * sizeof(float));
    memcpy(conv1dConfig->weight->p, weight, weightSize * sizeof(float));
    conv1dConfig->weight->grad = calloc(weightSize, sizeof(float));

    if (bias) {
        conv1dConfig->bias = malloc(sizeof(parameter_t));
        conv1dConfig->bias->size = outputChannels;
        conv1dConfig->bias->p = malloc(outputChannels * sizeof(float));
        memcpy(conv1dConfig->bias->p, bias, outputChannels * sizeof(float));
        conv1dConfig->bias->grad = calloc(outputChannels, sizeof(float));
        conv1dConfig->useBias = true;
    } else {
        conv1dConfig->bias = NULL;
        conv1dConfig->useBias = false;
    }

    return conv1dConfig;
}


conv1dConfig_t *initConv1dConfigWithKernels(parameter_t *kernels,
                                            float *bias,
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
    config->weight->grad = calloc(outputChannels * inputChannels * kernelSize,
                                  sizeof(float));

    if (bias) {
        config->useBias = true;
        config->bias = calloc(1, sizeof(parameter_t));
        config->bias->p = bias;
        config->bias->size = outputChannels;
        config->bias->grad = calloc(outputChannels, sizeof(float));
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

float *conv1dForward(void *config,
                     float *input,
                     size_t inputSize) {
    conv1dConfig_t *conv1dConfig = config;

    size_t inputChannels = conv1dConfig->inputChannels;
    size_t outputChannels = conv1dConfig->outputChannels;
    size_t kernelSize = conv1dConfig->kernelSize;
    size_t stride = conv1dConfig->stride;
    size_t dilation = conv1dConfig->dilation;
    size_t padding = conv1dConfig->padding->size;
    size_t inputSizePerChannel = inputSize / inputChannels;

    size_t outputSize = calcOutputSizeForInputSizeAndConv1dConfig(inputSize, config);
    size_t outputSizePerChannel = outputSize / outputChannels;

    float *output = calloc(outputChannels * outputSizePerChannel, sizeof(float));

    for (size_t oc = 0; oc < outputChannels; ++oc) {
        for (size_t i = 0; i < outputSizePerChannel; ++i) {
            float sum = 0.f;

            // For each input channel and each element of the kernel
            for (size_t ic = 0; ic < inputChannels; ++ic) {
                for (size_t k = 0; k < kernelSize; ++k) {
                    // compute the input index with stride, dilation, and padding
                    int inputIndex = (int)i * (int)stride
                                     + (int)k * (int)dilation
                                     - (int)padding;
                    // skip out-of-bounds (zero‐padding)
                    if (inputIndex < 0 || inputIndex >= (int)inputSizePerChannel) {
                        continue;
                    }

                    size_t input_idx = ic * inputSizePerChannel + (size_t)inputIndex;
                    size_t weight_idx = oc * inputChannels * kernelSize + ic * kernelSize + k;

                    sum += input[input_idx]
                        * conv1dConfig->weight->p[weight_idx];
                }
            }

            if (conv1dConfig->useBias) {
                sum += conv1dConfig->bias->p[oc];
            }

            output[oc * outputSizePerChannel + i] = sum;
        }
    }

    return output;
}


float *conv1dBackward(void *config,
                      float *gradOut,
                      float *input,
                      size_t inputSize) {
    conv1dConfig_t *conv1dConfig = config;

    size_t inputChannels = conv1dConfig->inputChannels;
    size_t outputChannels = conv1dConfig->outputChannels;
    size_t kernelSize = conv1dConfig->kernelSize;
    size_t stride = conv1dConfig->stride;
    size_t dilation = conv1dConfig->dilation;
    size_t padding = conv1dConfig->padding->size;

    size_t inputSizePerChannel = inputSize / inputChannels;
    size_t outputSize = calcOutputSizeForInputSizeAndConv1dConfig(inputSize, config);
    size_t outputSizePerChannel = outputSize / outputChannels;

    float *gradInput = calloc(inputSize, sizeof(float));

    // bias gradient: sum then accumulate normalized
    if (conv1dConfig->useBias) {
        for (size_t oc = 0; oc < outputChannels; ++oc) {
            float sumB = 0.f;
            for (size_t i = 0; i < outputSizePerChannel; ++i) {
                sumB += gradOut[oc * outputSizePerChannel + i];
            }
            conv1dConfig->bias->grad[oc] += sumB;
        }
    }

    // weight gradient & raw gradInput
    for (size_t oc = 0; oc < outputChannels; ++oc) {
        for (size_t ic = 0; ic < inputChannels; ic++) {
            for (size_t k = 0; k < kernelSize; k++) {
                for (size_t i = 0; i < outputSizePerChannel; i++) {
                    int inputIndex = (int)i * (int)stride + (int)k * (int)dilation - (int)padding;
                    if (inputIndex < 0 || inputIndex >= (int)inputSizePerChannel)
                        continue;

                    float g = gradOut[oc * outputSizePerChannel + i];

                    size_t u = ic * inputSizePerChannel + inputIndex;
                    size_t wIdx = oc * inputChannels * kernelSize + ic * kernelSize + k;

                    conv1dConfig->weight->grad[wIdx] += g * input[u];

                    gradInput[u] += g * conv1dConfig->weight->p[wIdx];

                }
            }
        }
    }

    /*for (size_t i = 0; i < inputSize; i++) {
        PRINT("output[%lu]: %f\n", i, gradInput[i]);
    }*/

    return gradInput;
}

layerForward_t *initConv1dLayerForward(float *weight, float *bias, size_t inputChannels,
                                       size_t outputChannels, size_t kernelSize, size_t stride,
                                       size_t dilation, paddingType_t paddingType,
                                       size_t paddingSize, size_t inputSize) {
    layerForward_t *layerForward = malloc(sizeof(layerForward_t));
    layerForward->config = initConv1dConfigWithWeightAndBias(
        weight, bias, inputChannels, outputChannels, kernelSize, stride, dilation, paddingType,
        paddingSize);
    layerForward->type = CONV1D;
    layerForward->inputSize = inputSize;

    return layerForward;
}

layerForwardBackward_t *initConv1dLayerForwardBackward(float *weight, float *bias,
                                                       size_t inputChannels,
                                                       size_t outputChannels, size_t kernelSize,
                                                       size_t stride,
                                                       size_t dilation, paddingType_t paddingType,
                                                       size_t paddingSize, size_t inputSize) {
    layerForwardBackward_t *layerForwardBackward = malloc(sizeof(layerForwardBackward_t));
    layerForwardBackward->config = initConv1dConfigWithWeightAndBias(
        weight, bias, inputChannels, outputChannels, kernelSize, stride, dilation, paddingType,
        paddingSize);
    layerForwardBackward->type = CONV1D;
    layerForwardBackward->inputSize = inputSize;

    return layerForwardBackward;
}
