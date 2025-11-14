#ifndef ENV5_RUNTIME_LAYER_H
#define ENV5_RUNTIME_LAYER_H

// TODO figure out how to do this, when forward func signatures aren't the same
// All void pointers? Do we lose something?
// Will have to cast everything

/*typedef void(forward)(void*, qTensor_t*);
typedef void(backward)(void*, qTensor_t*, qTensor_t*);

typedef void(loss)(qTensor_t*, qTensor_t*);*/

/*typedef struct
{
    forward* forwardFunc;
    backward* backwardFunc;
} layerFunctionEntry_t;*/

//extern const layerFunctionEntry_t layerFunctions[];


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

typedef struct layerForward
{
    void* config;
    layerType_t type;
} layerForward_t;

typedef struct layerForwardBackward
{
    layerType_t type;
    shape_t inferOutputShape(layerForwardBackward *self, shape_t input_shape);
    void* config;
} layerForwardBackward_t;

#endif // ENV5_RUNTIME_LAYER_H
