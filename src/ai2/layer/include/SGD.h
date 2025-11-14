#ifndef SGD_H
#define SGD_H
#include <stddef.h>
#include <stdint.h>
#include <Tensor.h>

typedef struct momentumBuffer
{
    tensor_t* dataTensor;
    tensor_t* gradTensor;
    float* momentums; // Pointer to array of momentums of size of data/gradData
} momentumBuffer_t;

typedef struct SGDConfig
{
    float lr; // factor for learning rate
    float momentum; // factor for momentum
    float weightDecay; // factor to decrease the weight
    momentumBuffer_t** momentum_buffer; // array of momentum buffers
    size_t sizeMomentumBuffers; //number of elements in momentum buffers
} SGDConfig_t;

void initMomentumBuffer(momentumBuffer_t* momentumBuffer, tensor_t* dataTensor, tensor_t* gradTensor, float* momentums);



#endif //SGD_H
