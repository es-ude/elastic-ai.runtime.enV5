#ifndef AIHELPERS_H
#define AIHELPERS_H

#include <stdlib.h>

/*! Generally every layer will have something like this
 *
 *
 * // Forward call for the layer that calculates the output of the layer for a given input
 * float *forward(layerConfig_t *config, float *input);
 *
 * //Backward call for the layer that calculates the gradients in respect to the inputs and in respect to the parameters
 * float *backward(layerConfig_t *config, float *grad, float *input);
 */

typedef float *(forward)(void*, float*);
typedef float *(backward)(void*, float*, float*);

/*! @brief Describes each parameter
 * p = array of parameter values
 * grad = array of size parameter in which the corresponding gradients can be summed up
 * size = size of array p & grad
 */
typedef struct parameter {
    float *p;
    float *grad;
    size_t size;
} parameter_t;

/*! @brief Init the parameter_t construct
 *
 * @param p
 * @param size
 * @return : pointer to parameter
 */
parameter_t *initParameter(float *p, size_t size);

typedef enum layerType {
    LINEAR,
    RELU,
    CONV1D
}layerType_t;

/*! @brief Describes how you can generally construct layers
 *
 * layerForward = pointer to forward function of the layer
 * config = config needed to execute the layer
 * type = type of layer
 */
typedef struct layerForward {
    forward *layerForward;
    void* config;
    layerType_t type;
    uint16_t inputSize;
}layerForward_t;

/*! @brief Computes Forward output for a given input and sequential network
 *
 * @param network: is array of layer_t structs
 * @param input: input for the neural network
 * @return : pointer to array of results
 */
float *sequentialForward(layerForward_t **network, size_t sizeNetwork,  float *input);

/*! @brief Describes how you can generally construct layers for Forward & Backward
 * layerForward = pointer to forward function of the layer
 * layerBackward = point to backward function of the layer
 * config = config needed to execute the layer
 * type = Type of layer
 */
typedef struct layerForwardBackward {
    forward *layerForward;
    backward *layerBackward;
    void* config;
    layerType_t type;
    uint16_t inputSize;
}layerForwardBackward_t;

typedef struct trainingStats {
    float *loss;
    float *output;
}trainingStats_t;

/*! @brief Computes Forward & Backward to calculate gradients for a given input & loss function
 *
 * @param network : Array of layerForwardBackward_t structs
 * @param lossFunction: Pointer to loss function that looks like lossFunction(float *output, float
 * *label, size_t size);
 * @param input : Array of inputs
 * @return returns result from trainingStats_t forward pass
 */
trainingStats_t *sequentialCalculateGrads(layerForwardBackward_t **network, size_t sizeNetwork, void* lossFunction, float *input, float *label);

#endif //AIHELPERS_H
