#include "Linear.h"
#include "Sequential.h"
#include "SGD.h"
#include "unity.h"

#include <stddef.h>

void testSequentialForwardLinearRelu() {
    parameter_t weights;
    tensor_t weightsParam;
    float weightData[] = {-1.f, 2.f, -3.f, 4.f, 5.f, 6.f};
    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;
    size_t weightOrderOfDims[] = {0, 1};
    shape_t weightShape = {.dimensions = weightDims,
                           .numberOfDimensions = weightNumberOfDims,
                           .orderOfDimensions = weightOrderOfDims};
    quantization_t weightQ;
    initFloat32Quantization(&weightQ);
    setTensorValues(&weightsParam, weightData, &weightShape, &weightQ, NULL);

    tensor_t weightsGrad;
    float weightGradData[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    quantization_t weightGradQ;
    initFloat32Quantization(&weightGradQ);
    setTensorValues(&weightsGrad, weightGradData, &weightShape, &weightQ, NULL);

    setParameterValues(&weights, &weightsParam, &weightsGrad);

    parameter_t bias;
    tensor_t biasParam;
    float biasData[] = {-1.f, 3.f};
    size_t biasDims[] = {2, 1};
    size_t biasNumberOfDims = 2;
    size_t biasOrderOfDims[] = {0, 1};
    shape_t biasShape = {.dimensions = biasDims,
                         .numberOfDimensions = biasNumberOfDims,
                         .orderOfDimensions = biasOrderOfDims};
    quantization_t biasQ;
    initFloat32Quantization(&biasQ);
    setTensorValues(&biasParam, biasData, &biasShape, &biasQ, NULL);

    tensor_t biasGrad;
    float biasGradData[] = {0.f, 0.f};
    quantization_t biasGradQ;
    initFloat32Quantization(&biasGradQ);
    setTensorValues(&biasGrad, biasGradData, &biasShape, &biasQ, NULL);
    setParameterValues(&bias, &biasParam, &biasGrad);

    tensor_t input;
    float inputData[] = {0.f, 1.f, 2.f};
    size_t inputDims[] = {1, 3};
    size_t inputNumberOfDims = 2;
    size_t inputOrderOfDims[] = {0, 1};
    shape_t inputShape = {.dimensions = inputDims,
                          .numberOfDimensions = inputNumberOfDims,
                          .orderOfDimensions = inputOrderOfDims};
    quantization_t inputQ;
    initFloat32Quantization(&inputQ);
    setTensorValues(&input, inputData, &inputShape, &inputQ, NULL);

    tensor_t output;
    float outputData[2] = {0, 0};
    size_t outputDims[] = {2, 1};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};
    shape_t outputShape = {.dimensions = outputDims,
                           .numberOfDimensions = outputNumberOfDims,
                           .orderOfDimensions = outputOrderOfDims};
    quantization_t outputQ;
    initFloat32Quantization(&outputQ);
    setTensorValues(&output, outputData, &outputShape, &outputQ, NULL);

    layer_t linear;
    layerConfig_t linearConfig;
    linearConfig_t linCfg;
    linearConfig.linear = &linCfg;
    linearInitConfig(linearConfig.linear, &weights, &bias);
    initLayer(&linear, LINEAR, &linearConfig, FLOAT_LAYER, &inputQ, &outputQ);

    layer_t relu;
    initLayer(&relu, RELU, NULL, FLOAT_LAYER, &inputQ, &outputQ);

    layer_t *model[] = {&linear, &relu};

    sequentialForward(model, 2, &input, &output);

    float expected[] = {0.f, 20.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, output.data, 2);
}

void testSequentialCalcGradsLinear() {
    parameter_t weights;
    tensor_t weightsParam;
    float weightData[] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
    size_t weightDims[] = {2, 3};
    size_t weightNumberOfDims = 2;
    size_t weightOrderOfDims[] = {0, 1};
    shape_t weightShape = {.dimensions = weightDims,
                           .numberOfDimensions = weightNumberOfDims,
                           .orderOfDimensions = weightOrderOfDims};
    quantization_t weightQ;
    initFloat32Quantization(&weightQ);
    setTensorValues(&weightsParam, weightData, &weightShape, &weightQ, NULL);

    tensor_t weightsGrad;
    float weightGradData[] = {0.f, 0.f, 0.f, 0.f, 0.f, 0.f};
    quantization_t weightGradQ;
    initFloat32Quantization(&weightGradQ);
    setTensorValues(&weightsGrad, weightGradData, &weightShape, &weightQ, NULL);
    setParameterValues(&weights, &weightsParam, &weightsGrad);

    parameter_t bias;
    tensor_t biasParam;
    float biasData[] = {-1.f, 3.f};
    size_t biasDims[] = {2, 1};
    size_t biasNumberOfDims = 2;
    size_t biasOrderOfDims[] = {0, 1};
    shape_t biasShape = {.dimensions = biasDims,
                         .numberOfDimensions = biasNumberOfDims,
                         .orderOfDimensions = biasOrderOfDims};
    quantization_t biasQ;
    initFloat32Quantization(&biasQ);
    setTensorValues(&biasParam, biasData, &biasShape, &biasQ, NULL);

    tensor_t biasGrad;
    float biasGradData[] = {0.f, 0.f};
    quantization_t biasGradQ;
    initFloat32Quantization(&biasGradQ);
    setTensorValues(&biasGrad, biasGradData, &biasShape, &biasQ, NULL);
    setParameterValues(&bias, &biasParam, &biasGrad);

    tensor_t input0;
    float input0Data[] = {-4.f, 1.f, 9.f,};
    size_t input0Dims[] = {1, 3};
    size_t input0NumberOfDims = 2;
    size_t input0OrderOfDims[] = {0, 1};
    shape_t input0Shape = {.dimensions = input0Dims,
                          .numberOfDimensions = input0NumberOfDims,
                          .orderOfDimensions = input0OrderOfDims};
    quantization_t input0Q;
    initFloat32Quantization(&input0Q);
    setTensorValues(&input0, input0Data, &input0Shape, &input0Q, NULL);

    tensor_t input1;
    float input1Data[] = {5.f, -1.f, 2.f};
    size_t input1Dims[] = {1, 3};
    size_t input1NumberOfDims = 2;
    size_t input1OrderOfDims[] = {0, 1};
    shape_t input1Shape = {.dimensions = input1Dims,
                          .numberOfDimensions = input1NumberOfDims,
                          .orderOfDimensions = input1OrderOfDims};
    quantization_t input1Q;
    initFloat32Quantization(&input1Q);
    setTensorValues(&input1, input1Data, &input1Shape, &input1Q, NULL);

    tensor_t input2;
    float input2Data[] = {-7.f, -5.f, 6.f};
    size_t input2Dims[] = {1, 3};
    size_t input2NumberOfDims = 2;
    size_t input2OrderOfDims[] = {0, 1};
    shape_t input2Shape = {.dimensions = input2Dims,
                          .numberOfDimensions = input2NumberOfDims,
                          .orderOfDimensions = input2OrderOfDims};
    quantization_t input2Q;
    initFloat32Quantization(&input2Q);
    setTensorValues(&input2, input2Data, &input2Shape, &input2Q, NULL);


    trainingStats_t trainingStats;

    tensor_t output;
    float outputData[2];
    size_t outputDims[] = {2, 1};
    size_t outputNumberOfDims = 2;
    size_t outputOrderOfDims[] = {0, 1};
    shape_t outputShape = {.dimensions = outputDims,
                           .numberOfDimensions = outputNumberOfDims,
                           .orderOfDimensions = outputOrderOfDims};
    quantization_t outputQ;
    initFloat32Quantization(&outputQ);
    setTensorValues(&output, outputData, &outputShape, &outputQ, NULL);

    tensor_t loss;
    float lossData[2];
    size_t lossDims[] = {2, 1};
    size_t lossNumberOfDims = 2;
    size_t lossOrderOfDims[] = {0, 1};
    shape_t lossShape = {.dimensions = lossDims,
                             .numberOfDimensions = lossNumberOfDims,
                             .orderOfDimensions = lossOrderOfDims};
    quantization_t lossQ;
    initFloat32Quantization(&lossQ);
    setTensorValues(&loss, lossData, &lossShape, &lossQ, NULL);

    trainingStats.loss = &loss;
    trainingStats.output = &output;

    layer_t linear;
    layerConfig_t linearConfig;
    linearConfig_t linCfg;
    linearConfig.linear = &linCfg;
    linearInitConfig(linearConfig.linear, &weights, &bias);
    initLayer(&linear, LINEAR, &linearConfig, FLOAT_LAYER, &input0Q, &outputQ);


    layer_t *model[] = {&linear};
    size_t sizeNetwork = 1;

    tensor_t label0;
    float label0Data[] = {59.f, -23.f};
    size_t label0Dims[] = {2, 1};
    size_t label0NumberOfDims = 2;
    size_t label0OrderOfDims[] = {0, 1};
    shape_t label0Shape = {.dimensions = label0Dims,
                          .numberOfDimensions = label0NumberOfDims,
                          .orderOfDimensions = label0OrderOfDims};
    quantization_t label0Q;
    initFloat32Quantization(&label0Q);
    setTensorValues(&label0, label0Data, &label0Shape, &label0Q, NULL);

    tensor_t label1;
    float label1Data[] = {43.f, 249.f};
    size_t label1Dims[] = {2, 1};
    size_t label1NumberOfDims = 2;
    size_t label1OrderOfDims[] = {0, 1};
    shape_t label1Shape = {.dimensions = label1Dims,
                          .numberOfDimensions = label1NumberOfDims,
                          .orderOfDimensions = label1OrderOfDims};
    quantization_t label1Q;
    initFloat32Quantization(&label1Q);
    setTensorValues(&label1, label1Data, &label1Shape, &label1Q, NULL);

    tensor_t label2;
    float label2Data[] = {23.f, 457.f};
    size_t label2Dims[] = {2, 1};
    size_t label2NumberOfDims = 2;
    size_t label2OrderOfDims[] = {0, 1};
    shape_t label2Shape = {.dimensions = label2Dims,
                          .numberOfDimensions = label2NumberOfDims,
                          .orderOfDimensions = label2OrderOfDims};
    quantization_t label2Q;
    initFloat32Quantization(&label2Q);
    setTensorValues(&label2, label2Data, &label2Shape, &label2Q, NULL);

    size_t sizeMomentumBuffers = 1;
    momentumBuffer_t *momentumBuffers[sizeMomentumBuffers];
    momentumBuffer_t weightMomentumBuffer;
    float weightMomentums[] = {0, 0, 0, 0, 0, 0};
    initMomentumBuffer(&weightMomentumBuffer, &weights, weightMomentums);

    momentumBuffers[0] = &weightMomentumBuffer;

    SGDConfig_t sgdConfig;
    initSGDConfig(&sgdConfig, 0.01f, 0.f, 0.f, momentumBuffers, sizeMomentumBuffers);


    for (size_t i = 0; i < 1000; i++) {
        sequentialCalculateGrads(model, sizeNetwork, MSE, &input0, &label0, &trainingStats);
        sequentialCalculateGrads(model, sizeNetwork, MSE, &input1, &label1, &trainingStats);
        sequentialCalculateGrads(model, sizeNetwork, MSE, &input2, &label2, &trainingStats);

        SGDStepFloat(&sgdConfig);
        SGDZeroGradFloat(&sgdConfig);
    }

    float expectedWeights[] = {5.f, -1.f, 9.f, 22.f, -100.f, 18.f};
    float *actualWeights = (float *)linearConfig.linear->weights->param->data;
    for (size_t i = 0; i < 6; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.01f, expectedWeights[i], actualWeights[i]);
    }
}


void setUp() {}
void tearDown() {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(testSequentialForwardLinearRelu);
    RUN_TEST(testSequentialCalcGradsLinear);
    UNITY_END();
}
