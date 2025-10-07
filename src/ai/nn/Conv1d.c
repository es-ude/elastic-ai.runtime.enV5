#define SOURCE_FILE "AI_NN_Conv1d"

#include "Conv1d.h"

#include <Common.h>
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

conv1dConfig_t *initConv1dConfigWithWeightAndBias(tensor_t weightTensor,
                                                  tensor_t biasTensor,
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
    memcpy(conv1dConfig->weight->p, weightTensor.data, weightSize * sizeof(float));
    conv1dConfig->weight->grad = calloc(weightSize, sizeof(float));

    if (biasTensor.data) {
        conv1dConfig->bias = malloc(sizeof(parameter_t));
        conv1dConfig->bias->size = outputChannels;
        conv1dConfig->bias->p = malloc(outputChannels * sizeof(float));
        memcpy(conv1dConfig->bias->p, biasTensor.data, outputChannels * sizeof(float));
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

float *conv1dForward(void *config, tensor_t *inputTensor) {
    conv1dConfig_t *conv1dConfig = config;

    size_t inChannels = conv1dConfig->inputChannels;
    size_t outChannels = conv1dConfig->outputChannels;
    size_t kernel = conv1dConfig->kernelSize;
    size_t stride = conv1dConfig->stride;
    size_t dilation = conv1dConfig->dilation;
    size_t inSizePerChannel = calcTotalNumberOfElementsByTensor(inputTensor) / inChannels;

    // SAME padding berechnen
    size_t outSizePerChannel;
    size_t padLeft = 0, padRight = 0;
    if (conv1dConfig->padding->type == SAME) {
        outSizePerChannel = (inSizePerChannel + stride - 1) / stride; // ceil
        size_t padNeeded = (outSizePerChannel - 1) * stride
                         + dilation * (kernel - 1) + 1
                         - inSizePerChannel;
        if ((int)padNeeded < 0) padNeeded = 0;
        padLeft  = padNeeded / 2;
        padRight = padNeeded - padLeft;
    } else { // VALID
        outSizePerChannel = (inSizePerChannel + 2 * conv1dConfig->padding->size
                             - dilation * (kernel - 1) - 1) / stride + 1;
        padLeft = conv1dConfig->padding->size;
        padRight = conv1dConfig->padding->size;
    }

    float *output = calloc(outChannels * outSizePerChannel, sizeof(float));

    for (size_t oc = 0; oc < outChannels; ++oc) {
        for (size_t i = 0; i < outSizePerChannel; ++i) {
            float sum = 0.f;
            for (size_t ic = 0; ic < inChannels; ++ic) {
                for (size_t k = 0; k < kernel; ++k) {
                    int inIndex = (int)i * (int)stride
                                + (int)k * (int)dilation
                                - (int)padLeft;
                    if (inIndex < 0 || inIndex >= (int)inSizePerChannel) continue;

                    size_t inIdx = ic * inSizePerChannel + (size_t)inIndex;
                    size_t wIdx  = oc * inChannels * kernel + ic * kernel + k;

                    sum += inputTensor->data[inIdx] * conv1dConfig->weight->p[wIdx];
                }
            }
            if (conv1dConfig->useBias) {
                sum += conv1dConfig->bias->p[oc];
            }
            output[oc * outSizePerChannel + i] = sum;
        }
    }
    return output;
}


tensor_t *conv1dBackward(void *config, tensor_t *gradTensor, tensor_t *inputTensor) {
    conv1dConfig_t *conv1dConfig = config;

    size_t inChannels = conv1dConfig->inputChannels;
    size_t outChannels = conv1dConfig->outputChannels;
    size_t kernel = conv1dConfig->kernelSize;
    size_t stride = conv1dConfig->stride;
    size_t dilation = conv1dConfig->dilation;
    size_t inputSize = calcTotalNumberOfElementsByTensor(inputTensor);
    size_t inSizePerChannel = inputSize / inChannels;

    // SAME padding berechnen
    size_t outSizePerChannel;
    size_t padLeft = 0, padRight = 0;
    if (conv1dConfig->padding->type == SAME) {
        outSizePerChannel = (inSizePerChannel + stride - 1) / stride;
        size_t padNeeded = (outSizePerChannel - 1) * stride
                         + dilation * (kernel - 1) + 1
                         - inSizePerChannel;
        if ((int)padNeeded < 0) padNeeded = 0;
        padLeft  = padNeeded / 2;
        padRight = padNeeded - padLeft;
    } else { // VALID
        outSizePerChannel = (inSizePerChannel + 2 * conv1dConfig->padding->size
                             - dilation * (kernel - 1) - 1) / stride + 1;
        padLeft = conv1dConfig->padding->size;
        padRight = conv1dConfig->padding->size;
    }

    tensor_t *gradInput = malloc(sizeof(tensor_t));
    gradInput->data = malloc(inputSize);
    gradInput->dimensions = malloc(inputTensor->numberOfDimensions * sizeof(size_t));
    gradInput->numberOfDimensions = inputTensor->numberOfDimensions;
    memcpy(gradInput->dimensions, inputTensor->dimensions, inputTensor->numberOfDimensions);

    // bias gradient
    if (conv1dConfig->useBias) {
        for (size_t oc = 0; oc < outChannels; ++oc) {
            float sumB = 0.f;
            for (size_t i = 0; i < outSizePerChannel; ++i) {
                sumB += gradTensor->data[oc * outSizePerChannel + i];
            }
            conv1dConfig->bias->grad[oc] += sumB;
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
                    if (inIndex < 0 || inIndex >= (int)inSizePerChannel) continue;

                    float g = gradTensor->data[oc * outSizePerChannel + i];

                    size_t inIdx = ic * inSizePerChannel + (size_t)inIndex;
                    size_t wIdx  = oc * inChannels * kernel + ic * kernel + k;

                    conv1dConfig->weight->grad[wIdx] += g * inputTensor->data[inIdx];
                    gradInput->data[inIdx] += g * conv1dConfig->weight->p[wIdx];
                }
            }
        }
    }

    return gradInput;
}

layerForward_t *initConv1dLayerForward(tensor_t weightTensor, tensor_t biasTensor, size_t inputChannels,
                                       size_t outputChannels, size_t kernelSize, size_t stride,
                                       size_t dilation, paddingType_t paddingType,
                                       size_t paddingSize, size_t inputSize) {
    layerForward_t *layerForward = malloc(sizeof(layerForward_t));
    layerForward->config = initConv1dConfigWithWeightAndBias(
        weightTensor, biasTensor, inputChannels, outputChannels, kernelSize, stride, dilation, paddingType,
        paddingSize);
    layerForward->type = CONV1D;
    layerForward->inputSize = inputSize;

    return layerForward;
}

layerForwardBackward_t *initConv1dLayerForwardBackward(tensor_t weightTensor, tensor_t biasTensor,
                                                       size_t inputChannels,
                                                       size_t outputChannels, size_t kernelSize,
                                                       size_t stride,
                                                       size_t dilation, paddingType_t paddingType,
                                                       size_t paddingSize, size_t inputSize) {
    layerForwardBackward_t *layerForwardBackward = malloc(sizeof(layerForwardBackward_t));
    layerForwardBackward->config = initConv1dConfigWithWeightAndBias(
        weightTensor, biasTensor, inputChannels, outputChannels, kernelSize, stride, dilation, paddingType,
        paddingSize);
    layerForwardBackward->type = CONV1D;
    layerForwardBackward->inputSize = inputSize;

    return layerForwardBackward;
}
