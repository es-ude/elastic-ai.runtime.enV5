#define SOURCE_FILE "AI_NN_Conv1d"

#include "Conv1d.h"

size_t calcOutputSizeForInputSizeAndConv1dConfig(size_t inputSize, conv1dConfig_t *conv1dConfig) {

}


conv1dConfig_t *initConv1dConfigWithKernels(parameter_t *kernels, size_t inputChannels, size_t outputChannels, size_t kernelSize, size_t stride, size_t dilation, paddingType_t paddingType, size_t paddingSize) {

}

conv1dConfig_t *initConv1dConfig(size_t inputChannels, size_t outputChannels, size_t kernelSize, size_t stride, size_t dilation, bool useBias, paddingType_t paddingType, size_t paddingSize){

}

float *conv1dForward(conv1dConfig_t *config, float *input) {

}

float *conv1dBackward(conv1dConfig_t *config, float *grad, float *input) {

}