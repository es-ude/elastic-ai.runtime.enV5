#ifndef AIHELPERS_H
#define AIHELPERS_H
#include "Quantization.h"

typedef qTensor_t*(forward)(void*, qTensor_t*);
typedef qTensor_t*(backward)(void*, qTensor_t*, qTensor_t*);

typedef qTensor_t*(loss)(qTensor_t*, qTensor_t*);

/*! @brief Enum of possible layer types
 */
typedef enum layerType
{
    LINEAR,
    RELU,
    CONV1D,
    SOFTMAX
} layerType_t;

typedef enum lossFunctionType
{
    MSE,
    CROSS_ENTROPY
} lossFunctionType_t;

typedef struct
{
    loss* lossFunction;
} lossFunctionEntry_t;


/*! @brief Pairs forward and backward functions of layer
 */
typedef struct
{
    forward* forwardFunc;
    backward* backwardFunc;
} layerFunctionEntry_t;

/*! @brief Array, that pairs enum layer types to forward/backward functions
 */
extern const layerFunctionEntry_t layerFunctions[];

/*! @brief Calculate total number of elements of given tensor
 *
 * @param qTensor
 * @return : total number of elements
 */
size_t calcTotalNumberOfElementsByTensor(qTensor_t* qTensor);

size_t calcTensorDataSize(size_t numberOfDimensions, size_t* dimensions);

/*! @brief Describes how you can generally construct layers
 *
 * layerForward = pointer to forward function of the layer
 * config = config needed to execute the layer
 * type = type of layer
 */
typedef struct layerForward
{
    void* config;
    layerType_t type;
} layerForward_t;

typedef struct flattenConfig {
    size_t size;
}flattenConfig_t;

layerForward_t *initFlattenLayerForward(qTensor_t *inputTensor);


    qTensor_t *flattenForward(void *config, qTensor_t *inputTensor);

/*! @brief Computes Forward output for a given input and sequential network
 *
 * @param network: is array of layer_t structs
 * @param input: input for the neural network
 * @return : pointer to array of results
 */
qTensor_t* sequentialForward(layerForward_t** network, size_t sizeNetwork, qTensor_t* input);

/*! @brief Describes how you can generally construct layers for Forward & Backward
 * layerForward = pointer to forward function of the layer
 * layerBackward = point to backward function of the layer
 * config = config needed to execute the layer
 * type = Type of layer
 */
typedef struct layerForwardBackward
{
    void* config;
    layerType_t type;
} layerForwardBackward_t;

/*! @brief Struct, that contains loss and output
 */
typedef struct trainingStats
{
    float* loss;
    float* output;
} trainingStats_t;

/*! @brief Computes Forward & Backward to calculate gradients for a given input & loss function
 *
 * @param network: Array of layerForwardBackward_t structs
 * @param sizeNetwork: Total number of layers
 * @param lossFunctionType: Enum type of loss function to be used
 * @param input: Tensor of inputs
 * @param label: Tensor of target
 * @return returns result from trainingStats_t forward pass
 */
trainingStats_t* sequentialCalculateGrads(layerForwardBackward_t** network,
                                          size_t sizeNetwork,
                                          lossFunctionType_t lossFunctionType,
                                          qTensor_t* input,
                                          qTensor_t* label);

#endif // AIHELPERS_H
