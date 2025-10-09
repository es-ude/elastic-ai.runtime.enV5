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
    parameterTensor_t *kernels;
}kernel_t;

typedef struct conv1dConfig {
    size_t inputChannels;
    size_t outputChannels;
    size_t outputSize;
    size_t kernelSize;
    parameterTensor_t *weight; // [channelIn][channelOut][kernelSize]
    parameterTensor_t *bias;
    bool useBias;
    size_t stride; // Wie viel verschieben wir den Kernel
    size_t dilation; // Abstand der Inputdaten für KernelMultiplikation
    padding_t *padding;
} conv1dConfig_t;

size_t calcOutputSizeForInputSizeAndConv1dConfig(size_t inputSize, conv1dConfig_t *conv1dConfig);

tensor_t *initConv1dOutputTensor(conv1dConfig_t *conv1dConfig, tensor_t *inputTensor);


conv1dConfig_t *initConv1dConfigWithWeightAndBias(parameterTensor_t *weightTensor, parameterTensor_t *biasTensor, size_t inputChannels, size_t outputChannels, size_t kernelSize, size_t stride, size_t dilation, paddingType_t paddingType, size_t paddingSize);

conv1dConfig_t *initConv1dConfig(size_t inputChannels, size_t outputChannels, size_t kernelSize, size_t stride, size_t dilation, bool useBias, paddingType_t paddingType, size_t paddingSize);

tensor_t *conv1dForward(void *config, tensor_t *inputTensor);

tensor_t *conv1dBackward(void *config, tensor_t *gradTensor, tensor_t *inputTensor);

layerForward_t *initConv1dLayerForward(parameterTensor_t *weightTensor, parameterTensor_t *biasTensor, size_t inputChannels,
                                      size_t outputChannels, size_t kernelSize, size_t stride,
                                      size_t dilation, paddingType_t paddingType,
                                      size_t paddingSize);

layerForwardBackward_t *initConv1dLayerForwardBackward(parameterTensor_t *weightTensor, parameterTensor_t *biasTensor, size_t inputChannels,
                                       size_t outputChannels, size_t kernelSize, size_t stride,
                                       size_t dilation, paddingType_t paddingType,
                                       size_t paddingSize);

#endif // ENV5_RUNTIME_CONV1D_H
