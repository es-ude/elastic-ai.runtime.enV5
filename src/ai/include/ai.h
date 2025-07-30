#ifndef ENV5_AI_HEADER
#define ENV5_AI_HEADER

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct parameter {
    float *p;
    float *grad;
    size_t size;
} parameter_t;

typedef struct layer {
    void* layer;
    void* config;
}layer_t;


float* sequentialForward(layer_t network[]);
float sequentialTraining(layer_t network[]);

// Generally every layer will have something like this
// float *forward(forwardConfig_t *config, float *input);
//float *backward(forwardConfig_t *config, float *grad, float *input);


// Linear
typedef struct linearConfig{
    size_t inputSize;
    size_t outputSize;
    parameter_t weight;
    parameter_t bias;
} linearConfig_t;

float* linearForward(linearConfig_t *config, float *input);
float* linearBackward(linearConfig_t *config, float *grad, float *input);


// ReLU
typedef struct ReLUConfig {
    size_t size;
}ReLUConfig_t;

float* ReLUForward(ReLUConfig_t *config, float *input);
float* ReLUBackward(ReLUConfig_t *config, float *grad, float *input);

//Loss Functions
float* MSELossDOutput(float* output, float* label, size_t size);

//OPTIM

// Momentum is same Size as Parameter struct
typedef struct momentumBuffer {
    void *parameter; // Pointer to ONE parameter struct: Bin mir gerade nicht sicher, ob das so mit dem * so muss
    float *momentums; // Array of momentums of size of parameter
} momentumBuffer_t;

typedef struct SGDConfig {
    float lr; // factor for learning rate
    float momentum; // factor for momentum
    float weightDecay; // factor to decrease the weight
    momentumBuffer_t *momentum_buffer; // array of momentum buffers
    size_t sizeMomentumBuffers;
} SGDConfig_t;

// Perform all updates elementwise in this sequence. Also, see https://docs.pytorch.org/docs/stable/generated/torch.optim.SGD.html
// grad = grad + weightDecay * parameter
// momentumBuffer = momentum * momentumBuffer + grad
// parameter = parameter - lr * momentumBuffer
void SGDStep(SGDConfig_t *SGDConfig);
void SGDZeroGrad(SGDConfig_t *SGDConfig);

#endif