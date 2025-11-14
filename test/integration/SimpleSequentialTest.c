#include "Tensor.h"

typedef struct sequential sequential_t;

struct sequential {
    layer_t parent;
    void (append)(sequential_t* self, layer_t *otherLayer);
    int (getSize)(const sequential_t* self);
};

void forwardSequential(sequential_t *layers, tensor_t* inputTensor, tensor_t *outputTensor) {


    for(size_t i = 0; i < layers->getSize(layers); i++) {
        layer_t currentLayer = layerList[i];
        // initOutputTensorByLayer

        currentLayer.Forward(inputTensor, outputTensor);


    }
}

int main(void) {

}