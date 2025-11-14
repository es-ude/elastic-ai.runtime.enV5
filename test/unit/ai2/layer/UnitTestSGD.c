#define SOURCE_FILE "SGD-UTEST"

#include "SGD.h"
#include "Linear.h"
#include "Relu.h"
#include "unity.h"
#include "Layer.h"

#include <stdlib.h>
#include <string.h>

void setUp() {}
void tearDown() {}

void unitTestInitMomentumBuffer() {
    size_t numberOfValues = 3;
    float p[] = {0.f, 1.f, 2.f};
    size_t pDims[] = {numberOfValues};
    size_t pNumberOfDims = 1;
    size_t pOrderOfDims[] = {0};
    quantization_t paramQ;
    initFloat32Quantization(&paramQ);
    float *pGrads[numberOfValues];
    quantization_t pGradQ;
    initFloat32Quantization(&pGradQ);

    parameter_t param;
    setParameterValues(&param, p, &paramQ, pGrads, &pGradQ, pDims, pNumberOfDims, pOrderOfDims, NULL);

    float momentums[] = {2.f, 1.f, 0.f};
    momentumBuffer_t momentumBuffer;
    initMomentumBuffer(&momentumBuffer, &param, momentums);

    float expectedMomentumBuffer[] = {2.f, 1.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedMomentumBuffer, momentumBuffer.momentums, 3);
    TEST_ASSERT_EQUAL_PTR(p, momentumBuffer.parameter->tensor.data);
}

void unitTestInitSGDConfig() {
    parameter_t weights;
    size_t numberOfWeights = 3;
    float weightData[] = {0.f, 1.f, 2.f};
    size_t weightDims[] = {numberOfWeights};
    size_t weightNumberOfDims = 1;
    size_t weightOrderOfDims[] = {0};
    quantization_t weightQ;
    initFloat32Quantization(&weightQ);
    float weightGrads[] = {0.f, 0.f, 0.f};
    quantization_t weightGradsQ;
    initFloat32Quantization(&weightGradsQ);
    setParameterValues(&weights, weightData, &weightQ, weightGrads, &weightGradsQ, weightDims, weightNumberOfDims, weightOrderOfDims, NULL);

    parameter_t bias;
    size_t numberOfBiases = 3;
    float biasData[] = {0.f, 1.f, -1.f};
    size_t biasDims[] = {numberOfBiases};
    size_t biasNumberOfDims = 1;
    size_t biasOrderOfDims[] = {0};
    quantization_t biasQ;
    initFloat32Quantization(&biasQ);
    quantization_t biasGradQ;
    initFloat32Quantization(&biasGradQ);
    float biasGrads[] = {0.f, 0.f, 0.f};
    setParameterValues(&bias, biasData, &biasQ, biasGrads, &biasGradQ, biasDims, biasNumberOfDims, biasOrderOfDims, NULL);


    layer_t linear0;
    linearConfig_t linearConfig;
    initLinearConfig(&linearConfig, FLOATLAYER, &weights, &bias);
    initLinearLayer(&linear0, &linearConfig);

    layer_t relu0;
    initReluLayer(&relu0);

    layer_t linear1;
    initLinearLayer(&linear1, &linearConfig);

    layer_t model[3] = {linear0, relu0, linear1};
    size_t sizeModel = sizeof(model) / sizeof(model[0]);
    float lr = 0.1f;
    float momentumFactor = 0.9f;
    float weightDecay = 0.5f;

    size_t sizeMomentumBuffers = calcTotalNumberOfMomentumBuffers(model, sizeModel);

    float momentum[] = {0.f, 0.f, 0.f};

    momentumBuffer_t momentumBuffers[sizeMomentumBuffers];
    momentumBuffers[0].parameter = &weights;
    momentumBuffers[0].momentums = momentum;

    momentumBuffers[1].parameter = &bias;
    momentumBuffers[1].momentums = momentum;

    momentumBuffers[2].parameter = &weights;
    momentumBuffers[2].momentums = momentum;

    momentumBuffers[3].parameter = &bias;
    momentumBuffers[3].momentums = momentum;


    SGDConfig_t sgdConfig;
    initSGDConfig(&sgdConfig, lr, momentumFactor, weightDecay, momentumBuffers, sizeMomentumBuffers);

    linearConfig_t *linear0Conf = linear0.layerConfig;
    linearConfig_t *linear1Conf = linear1.layerConfig;



    TEST_ASSERT_EQUAL_FLOAT(lr, sgdConfig.learningRate);
    TEST_ASSERT_EQUAL_FLOAT(momentumFactor, sgdConfig.momentumFactor);
    TEST_ASSERT_EQUAL_FLOAT(weightDecay, sgdConfig.weightDecay);
    TEST_ASSERT_EQUAL_size_t(4, sgdConfig.sizeMomentumBuffers);

    TEST_ASSERT_EQUAL_PTR(linear0Conf->weights, sgdConfig.momentumBuffers[0].parameter);
    TEST_ASSERT_EQUAL_PTR(linear0Conf->bias, sgdConfig.momentumBuffers[1].parameter);
    TEST_ASSERT_EQUAL_PTR(linear1Conf->weights, sgdConfig.momentumBuffers[2].parameter);
    TEST_ASSERT_EQUAL_PTR(linear1Conf->bias, sgdConfig.momentumBuffers[3].parameter);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear0Conf->weights->grad, sgdConfig.momentumBuffers[0].momentums,
                                  calcNumberOfElementsByParameter(linear0Conf->weights));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear0Conf->bias->grad, sgdConfig.momentumBuffers[1].momentums,
                                  calcNumberOfElementsByParameter(linear0Conf->bias));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear1Conf->weights->grad, sgdConfig.momentumBuffers[2].momentums,
                              calcNumberOfElementsByParameter(linear1Conf->weights));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear1Conf->bias->grad, sgdConfig.momentumBuffers[3].momentums,
                                  calcNumberOfElementsByParameter(linear1Conf->bias));
}


void unitTestSGDStep() {

    parameter_t weights;
    size_t numberOfWeights = 3;
    float weightData[] = {1.f, 2.f, -3.f};
    size_t weightDims[] = {numberOfWeights};
    size_t weightNumberOfDims = 1;
    size_t weightOrderOfDims[] = {0};
    quantization_t weightQ;
    initFloat32Quantization(&weightQ);
    float weightGrads[] = {1.f, -1.f, 2.f};
    quantization_t weightGradsQ;
    initFloat32Quantization(&weightGradsQ);
    setParameterValues(&weights, weightData, &weightQ, weightGrads, &weightGradsQ, weightDims, weightNumberOfDims, weightOrderOfDims, NULL);


    parameter_t bias;
    size_t numberOfBiases = 2;
    float biasData[] = {-1.f, 3.f};
    size_t biasDims[] = {numberOfBiases};
    size_t biasNumberOfDims = 1;
    size_t biasOrderOfDims[] = {0};
    quantization_t biasQ;
    initFloat32Quantization(&biasQ);
    quantization_t biasGradQ;
    initFloat32Quantization(&biasGradQ);
    float biasGrads[] = {1.f, 3.f};
    setParameterValues(&bias, biasData, &biasQ, biasGrads, &biasGradQ, biasDims, biasNumberOfDims, biasOrderOfDims, NULL);

    layer_t linear0;
    linearConfig_t linearConfig;
    initLinearConfig(&linearConfig, FLOATLAYER, &weights, &bias);
    initLinearLayer(&linear0, &linearConfig);

    layer_t model[] = {linear0};
    float lr = 0.1f;
    float momentumFactor = 0.9f;
    float weightDecay = 0.01f;

    size_t sizeMomentumBuffers = calcTotalNumberOfMomentumBuffers(model, 1);
    float momentumWeights[] = {0.f, 0.f, 0.f};
    float momentumBias[] = {0.f, 0.f, 0.f};


    momentumBuffer_t momentumBuffers[sizeMomentumBuffers];
    momentumBuffers[0].parameter = &weights;
    momentumBuffers[0].momentums = momentumWeights;

    momentumBuffers[1].parameter = &bias;
    momentumBuffers[1].momentums = momentumBias;

    SGDConfig_t config;
    initSGDConfig(&config, lr, momentumFactor, weightDecay, momentumBuffers, sizeMomentumBuffers);

    SGDStepFloat(&config);

    float wPExpected[] = {0.899f, 2.098f, -3.197f};
    float bPExpected[] = {-1.099f, 2.697f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wPExpected, linearConfig.weights->tensor.data,
                                  sizeof(wPExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bPExpected, linearConfig.bias->tensor.data,
                                  sizeof(bPExpected)/sizeof(float));

    // Second Step with same grads but with momentum now
    SGDStepFloat(&config);

    float wPExpected2[] = {0.707201f, 2.284102f, -3.571103f};
    float bPExpected2[] = {-1.287001f, 2.121603f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wPExpected2, linearConfig.weights->tensor.data,
                                  sizeof(wPExpected2)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bPExpected2, linearConfig.bias->tensor.data,
                                  sizeof(bPExpected2)/sizeof(float));
}

void unitTestSGDZeroGrad() {
    parameter_t weights;
    size_t numberOfWeights = 3;
    float weightData[] = {1.f, 2.f, -3.f};
    size_t weightDims[] = {numberOfWeights};
    size_t weightNumberOfDims = 1;
    size_t weightOrderOfDims[] = {0};
    quantization_t weightQ;
    initFloat32Quantization(&weightQ);
    float weightGrads[] = {1.f, -1.f, 2.f};
    quantization_t weightGradsQ;
    initFloat32Quantization(&weightGradsQ);
    setParameterValues(&weights, weightData, &weightQ, weightGrads, &weightGradsQ, weightDims, weightNumberOfDims, weightOrderOfDims, NULL);


    parameter_t bias;
    size_t numberOfBiases = 2;
    float biasData[] = {-1.f, 3.f};
    size_t biasDims[] = {numberOfBiases};
    size_t biasNumberOfDims = 1;
    size_t biasOrderOfDims[] = {0};
    quantization_t biasQ;
    initFloat32Quantization(&biasQ);
    quantization_t biasGradQ;
    initFloat32Quantization(&biasGradQ);
    float biasGrads[] = {1.f, 3.f};
    setParameterValues(&bias, biasData, &biasQ, biasGrads, &biasGradQ, biasDims, biasNumberOfDims, biasOrderOfDims, NULL);


    layer_t linear0;
    linearConfig_t linearConfig;
    initLinearConfig(&linearConfig, FLOATLAYER, &weights, &bias);
    initLinearLayer(&linear0, &linearConfig);

    layer_t model[] = {linear0};
    float lr = 0.1f;
    float momentumFactor = 0.9f;
    float weightDecay = 0.01f;

    size_t sizeMomentumBuffers = calcTotalNumberOfMomentumBuffers(model, 1);
    float momentumWeights[] = {0.f, 0.f, 0.f};
    float momentumBias[] = {0.f, 0.f, 0.f};


    momentumBuffer_t momentumBuffers[sizeMomentumBuffers];
    momentumBuffers[0].parameter = &weights;
    momentumBuffers[0].momentums = momentumWeights;

    momentumBuffers[1].parameter = &bias;
    momentumBuffers[1].momentums = momentumBias;

    SGDConfig_t sgdConfig;
    initSGDConfig(&sgdConfig, lr, momentumFactor, weightDecay, momentumBuffers, sizeMomentumBuffers);

    SGDZeroGradFloat(&sgdConfig);
    float wGradExpected[] = {0.f, 0.f, 0.f};
    float bGradExpected[] = {0.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wGradExpected, weights.grad, sizeof(wGradExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bGradExpected, bias.grad, sizeof(bGradExpected)/sizeof(float));

}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestSGDStep);
    RUN_TEST(unitTestSGDZeroGrad);
    RUN_TEST(unitTestInitMomentumBuffer);
    RUN_TEST(unitTestInitSGDConfig);
    UNITY_END();
}
