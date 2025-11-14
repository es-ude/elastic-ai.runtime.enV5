
#ifndef ENV5_RUNTIME_LAYER_H
#define ENV5_RUNTIME_LAYER_H
#include "Tensor.h"

#include <stddef.h>

// TODO figure out how to do this, when forward func signatures aren't the same
// All void pointers? Do we lose something?
// Will have to cast everything

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


typedef struct layer layer_t;

typedef void(*calcOutputShapeFn_t)(layer_t* self, shape_t* inputShape, shape_t* outputShape);
typedef void(*forwardFn_t)(void* config, tensor_t* inputTensor, tensor_t* outputTensor);
typedef void(*backwardFn_t)(void* config, tensor_t* inputTensor, tensor_t* outputTensor);

struct layer
{
    layerType_t type;
    calcOutputShapeFn_t calcOutputShape;
    forwardFn_t forward;
    backwardFn_t backward;
    void* layerConfig;
};

void calcOutputShape(layer_t* layer, shape_t* inputShape, shape_t* outputShape);


#endif
