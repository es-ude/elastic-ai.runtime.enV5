#ifndef AIHELPERS_H
#define AIHELPERS_H

#include <stdint.h>
#include <stdlib.h>

/*! Generally every layer will have something like this
 *
 *
 * // Forward call for the layer that calculates the output of the layer for a given input
 * float *forward(layerConfig_t *config, float *input);
 *
 * //Backward call for the layer that calculates the gradients in respect to the inputs and in
 * respect to the parameters float *backward(layerConfig_t *config, float *grad, float *input);
 */
// qtype enum= FIXEDPOINT, FLOAT64, FLOAT32, FLOAT16,

//Arithmetic formats
//float, double, int8_t, int16_t, int32_t, int64_t;

/*! @brief Enum of possible quantization types
 */
typedef enum qtype
{
    FIXEDPOINT,
    FLOAT16,
    FLOAT32,
    FLOAT64
} qtype_t;

typedef struct fixedPointQ
{
    size_t int_bits;
    size_t frac_bits;
} fixedPointQ_t;

typedef struct floatQ
{
    size_t mantissa_bits;
    size_t exponent_bits;
} floatQ_t;

typedef struct quantization
{
    qtype_t type;
    void* qConfig;
} quantization_t;

typedef struct qTensor
{
    uint8_t* data;
    quantization_t* quantization;
    size_t* numberOfDimensions;
    size_t dimensions;
} qTensor_t;

typedef struct tensor
{
    float* data;
    size_t numberOfDimensions;
    size_t* dimensions;
} tensor_t;

/*! @brief Describes a parameter tensor
 * @param tensor = base tensor, containing data, numberOfDimensions and dimensions
 * @param grad = array of same size as data array in base tensor, in which the corresponding gradients can be summed up
 */
typedef struct parameterTensor
{
    struct tensor* tensor;
    float* grad;
} parameterTensor_t;

typedef tensor_t*(forward)(void*, tensor_t*);
typedef tensor_t*(backward)(void*, tensor_t*, tensor_t*);

typedef tensor_t*(loss)(tensor_t*, tensor_t*);

/*! @brief Enum of possible layer types
 */
typedef enum layerType
{
    LINEAR,
    RELU,
    CONV1D,
    SOFTMAX,
    FLATTEN
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
 * @param tensor
 * @return : total number of elements
 */
size_t calcTotalNumberOfElementsByTensor(tensor_t* tensor);

size_t calcTensorDataSize(size_t numberOfDimensions, size_t* dimensions);

/*! @brief Init tensor
 * @param data : array of data
 * @param numberOfDimensions : number of dimensions
 * @param dimensions : array of dimensions
 * @return : tensor
 */
tensor_t* initTensor(float* data, size_t numberOfDimensions, size_t* dimensions);

/*! @brief Init tensor with grads
 * @param data : array of data
 * @param numberOfDimensions : number of dimensions
 * @param dimensions : array of dimensions
 * @return : parameter tensor
 */
parameterTensor_t* initParameterTensor(float* data, size_t numberOfDimensions, size_t* dimensions);

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

layerForward_t *initFlattenLayerForward(tensor_t *inputTensor);


    tensor_t *flattenForward(void *config, tensor_t *inputTensor);

/*! @brief Computes Forward output for a given input and sequential network
 *
 * @param network: is array of layer_t structs
 * @param input: input for the neural network
 * @return : pointer to array of results
 */
tensor_t* sequentialForward(layerForward_t** network, size_t sizeNetwork, tensor_t* input);

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
 * @param network : Array of layerForwardBackward_t structs
 * @param lossFunction: Pointer to loss function that looks like lossFunction(float *output, float
 * *label, size_t size);
 * @param input : Array of inputs
 * @return returns result from trainingStats_t forward pass
 */
trainingStats_t* sequentialCalculateGrads(layerForwardBackward_t** network,
                                          size_t sizeNetwork,
                                          lossFunctionType_t lossFunctionType,
                                          tensor_t* input,
                                          tensor_t* label);

#endif // AIHELPERS_H
