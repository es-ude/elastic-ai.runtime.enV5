#ifndef ENV5_RUNTIME_CONV1D_H
#define ENV5_RUNTIME_CONV1D_H
#include "AiHelpers.h"

#include <stdbool.h>
#include <stdio.h>

typedef enum paddingType {
    NONE,
    ZEROS,
    SAME
}paddingType_t;

typedef struct padding {
    paddingType_t type;
    size_t size;
}padding_t;

typedef struct kernels {
    parameter_t *kernels;
}kernel_t;

typedef struct conv1dConfig {
    size_t inputChannels;
    size_t outputChannels;
    size_t kernelSize;
    parameter_t *weight; // [channelIn][channelOut][kernelSize]
    parameter_t *bias;
    bool useBias;
    size_t stride; // Wie viel verschieben wir den Kernel
    size_t dilation; // Abstand der Inputdaten für KernelMultiplikation
    padding_t *padding;
} conv1dConfig_t;

size_t calcOutputSizeForInputSizeAndConv1dConfig(size_t inputSize, conv1dConfig_t *conv1dConfig);

conv1dConfig_t *initConv1dConfigWithWeightAndBias(float *weight, float *bias, size_t inputChannels, size_t outputChannels, size_t kernelSize, size_t stride, size_t dilation, paddingType_t paddingType, size_t paddingSize);

conv1dConfig_t *initConv1dConfig(size_t inputChannels, size_t outputChannels, size_t kernelSize, size_t stride, size_t dilation, bool useBias, paddingType_t paddingType, size_t paddingSize);

float *conv1dForward(void *config, float *input, size_t inputSize);

float *conv1dBackward(void *config, float *grad, float *input, size_t inputSize);

#endif // ENV5_RUNTIME_CONV1D_H
