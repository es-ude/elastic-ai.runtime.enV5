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

#endif