#ifndef ENV5_AI_HEADER
#define ENV5_AI_HEADER

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
    RELU
}layerType_t;

/*! @brief Describes how you can generally construct layers
 *
 * layerForward = pointer to forward function of the layer
 * config = config needed to execute the layer
 * type = type of layer
 */
typedef struct layerForward {
    void* layerForward;
    void* config;
    layerType_t type;
}layerForward_t;

/*! @brief Computes Forward output for a given input and sequential network
 *
 * @param network: is array of layer_t structs
 * @param input: input for the neural network
 * @return : pointer to array of results
 */
float *sequentialForward(layerForward_t network[], float *input);

/*! @brief Describes how you can generally construct layers for Forward & Backward
 * layerForward = pointer to forward function of the layer
 * layerBackward = point to backward function of the layer
 * config = config needed to execute the layer
 * type = Type of layer
 */
typedef struct layerForwardBackward {
    void* layerForward;
    void* layerBackward;
    void* config;
    layerType_t type;
}layerForwardBackward_t;

/*! @brief Computes Forward & Backward to calculate gradients for a given input & loss function
 *
 * @param network : Array of layerForwardBackward_t structs
 * @param lossFunction: Pointer to loss function that looks like lossFunction(float *output, float
 * *label, size_t size);
 * @param input : Array of inputs
 */
void sequentialCalculateGrads(layerForwardBackward_t network[], void* lossFunction, float *input);


typedef struct linearConfig{
    size_t inputSize;
    size_t outputSize;
    parameter_t weight;
    parameter_t bias;
} linearConfig_t;

/*! @brief Gets linearConfig_t for a given weight & bias
 *
 * @param weight : pointer to array of weight of linear layer
 * @param sizeWeights: number of weights in array
 * @param bias : pointer to array of bias of linear layer
 * @param sizeBias : number of bias in array
 * @return: pointer to linearConfig_t
 */
linearConfig_t *initLinearConfigWithWeightBias(float *weight, size_t sizeWeights, float *bias, size_t sizeBias);

/*! @brief Return pointer to linearConfig_t with automatic weight and bias instantiation, likely glorot
 *
 * init Low Prio!
 * @param inputSize : input size of linear layer
 * @param outputSize : output size of linear layer
 * @return : pointer to linearConfig_t
 */
linearConfig_t *initLinearConfigWithInputOutputSize(size_t inputSize, size_t outputSize);

/*! @brief Returns pointer to layerForward_t
 *
 * @param weight : pointer to array of weights of the linear layer
 * @param sizeWeights: size of array of weights
 * @param bias : pointer to array of bias of the linear layer
 * @param sizeBias : size of array of weights
 * @return : pointer to layerForward_t
 */
layerForward_t *initLinearLayerForwardWithWeightBias(float *weight, size_t sizeWeights, float *bias, size_t sizeBias);

/*! @brief Return point to layerForward_t for linear layer
 * linearConfig_t with automatic weight and bias instatiation, likely glorot init
 * Low Prio
 *
 * @param inputSize : inputSize of linear layer
 * @param outputSize : outputSize of linear layer
 * @return : pointer to layerForward_t
 */
layerForward_t *initLinearLayerWithInputOutputSize(size_t inputSize, size_t outputSize);

/*! @brief Returns pointer to layerForward_t for a given weight and bias
 *
 * @param weight : pointer to array of weights of the linear layer
 * @param sizeWeights: size of array of weights
 * @param bias : pointer to array of bias of the linear layer
 * @param sizeBias : size of array of weights
 * @return : pointer to layerForwardBackward_t
 */
layerForwardBackward_t *initLinearLayerBackwardWithWeightBias(float *weight, size_t sizeWeights, float *bias, size_t sizeBias);

/*! @brief Return point to layerForward_t for linear layer
 * linearConfig_t with automatic weight and bias instatiation, likely glorot init
 * Low Prio
 *
 * @param inputSize : inputSize of linear layer
 * @param outputSize : outputSize of linear layer
 * @return : pointer to layerForwardBackward_t
 */
layerForwardBackward_t *initLinearLayerBackwardWithInputOutputSize(size_t inputSize, size_t outputSize);

/*! @brief Forward call for linear layer
 *
 * @param config : Config of linear layer
 * @param input : inputs for the linear layer
 * @return : output of the linear layer
 */
float *linearForward(linearConfig_t *config, float *input);

/*! @brief Backward call for the linear layer that calculates the gradients in respect to the inputs
 * and in respect to the parameters
 *
 * @param config : Config of linear layer
 * @param grad : partial gradients of loss function till here
 * @param input : input that was put into the layer
 * @return : partial gradients of loss function for the previous layers
 */
float *linearBackward(linearConfig_t *config, float *grad, float *input);


// ReLU
typedef struct ReLUConfig {
    size_t size;
}ReLUConfig_t;

/*! @brief
 *
 * @param size : Number of values
 * @return : Pointer to ReLUConfig
 */
ReLUConfig_t *initReLUConfig(size_t size);

/*! @brief Forward call for the ReLU layer
 *
 * @param config : gives the number of inputs
 * @param input : give the inputs
 * @return : calculates the output
 */
float * ReLUForward(ReLUConfig_t *config, float *input);

/*! @brief Backward call for the ReLU layer calculating the gradients in respect to the inputs
 *
 * @param config : gives the number of inputs
 * @param grad : partial gradients of loss function till here
 * @param input : input that was put into the layer
 * @return : partial gradients of loss function for the previous layers
 */
float * ReLUBackward(ReLUConfig_t *config, float *grad, float *input);

// Loss Functions

/*! @brief Calculates the Derivative of the MSE Loss in respect to the output.
 *
 * @param output : array of outputs
 * @param label : array of labels
 * @param size : size of outputs and labels
 * @return : derivative of MSE in respect to the output
 */
float * MSELossDOutput(float* output, float* label, size_t size);

//OPTIM

// Momentum is same Size as Parameter struct
typedef struct momentumBuffer {
    void *parameter; // Pointer to ONE parameter struct: Bin mir gerade nicht sicher, ob das so mit dem * so muss
    float *momentums; // Array of momentums of size of parameter
} momentumBuffer_t;

/*! @brief initalizes the momentumBuffer for a given parameter
 *
 * @param parameter: pointer to parameter
 * @return : Pointer to momentumBuffer
 */
momentumBuffer_t *initMomentumBuffer(void *parameter);

typedef struct SGDConfig {
    float lr; // factor for learning rate
    float momentum; // factor for momentum
    float weightDecay; // factor to decrease the weight
    momentumBuffer_t *momentum_buffer; // array of momentum buffers
    size_t sizeMomentumBuffers; //number of elements in momentum buffers
} SGDConfig_t;

/*! @brief Initalizes the SGDConfig for a given array of layerForwardBackward with lr, momentum &
 * weightDecay
 *
 * @param model : Pointer to array of type layerForwardBackward_t
 * @param sizeModel : Size of the array
 * @param lr : learning rate
 * @param momentum : momentum factor
 * @param weightDecay : weight decay factor
 * @return : Pointer to SGDConfig
 */
SGDConfig_t *initSGDConfig(layerForwardBackward_t *model, size_t sizeModel, float lr, float momentum, float weightDecay);

/*! @brief SGD Step implementation
 * Perform all updates elementwise in this sequence. Also, see
 * https://docs.pytorch.org/docs/stable/generated/torch.optim.SGD.html
 * grad = grad + weightDecay * parameter
 * momentumBuffer = momentum * momentumBuffer + grad
 * parameter = parameter - lr * momentumBuffer
 *
 * @param SGDConfig : Pointer to SGDConfig.
 */
void SGDStep(SGDConfig_t *SGDConfig);

/*! @brief Zeros the gradients for a given SGDConfig
 *
 * @param SGDConfig : Pointer to SGDConfig
 */
void SGDZeroGrad(SGDConfig_t *SGDConfig);

#endif