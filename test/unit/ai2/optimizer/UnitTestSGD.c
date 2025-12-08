#define SOURCE_FILE "SGD-UTEST"

#include "SGD.h"
#include "Linear.h"
#include "unity.h"
#include "Layer.h"
#include "Tensor.h"
#include "Rounding.h"

#include <stdlib.h>
#include <string.h>

void setUp() {}
void tearDown() {}

void unitTestInitMomentumBuffer() {
    parameter_t parameter;
    tensor_t param;
    size_t numberOfValues = 3;
    float p[] = {0.f, 1.f, 2.f};
    size_t pDims[] = {numberOfValues};
    size_t pNumberOfDims = 1;
    size_t pOrderOfDims[] = {0};
    shape_t pShape = {
        .dimensions = pDims,
        .numberOfDimensions = pNumberOfDims,
        .orderOfDimensions = pOrderOfDims
    };
    quantization_t paramQ;
    initFloat32Quantization(&paramQ);
    float *pGrads[numberOfValues];
    quantization_t pGradQ;
    initFloat32Quantization(&pGradQ);
    setTensorValues(&param, p, &pShape, &paramQ, NULL);
    setParameterValues(&parameter, &param, NULL);

    float momentums[] = {2.f, 1.f, 0.f};
    momentumBuffer_t momentumBuffer;
    initMomentumBuffer(&momentumBuffer, &parameter, momentums);

    float expectedMomentumBuffer[] = {2.f, 1.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expectedMomentumBuffer, momentumBuffer.momentums, 3);
    TEST_ASSERT_EQUAL_PTR(p, momentumBuffer.parameter->param->data);
}

void unitTestInitSGDConfig() {
    parameter_t weights;

    tensor_t weightParam;
    size_t numberOfWeights = 3;
    float weightData[] = {0.f, 1.f, 2.f};
    size_t weightDims[] = {numberOfWeights};
    size_t weightNumberOfDims = 1;
    size_t weightOrderOfDims[] = {0};
    shape_t weightShape = {
        .dimensions = weightDims,
        .numberOfDimensions = weightNumberOfDims,
        .orderOfDimensions = weightOrderOfDims
    };
    quantization_t weightQ;
    initFloat32Quantization(&weightQ);
    setTensorValues(&weightParam, weightData, &weightShape, &weightQ, NULL);

    tensor_t weightGrad;
    float weightGradData[] = {0.f, 0.f, 0.f};
    quantization_t weightGradQ;
    initFloat32Quantization(&weightGradQ);
    setTensorValues(&weightGrad, weightGradData, &weightShape, &weightGradQ, NULL);
    setParameterValues(&weights, &weightParam, &weightGrad);

    parameter_t bias;
    tensor_t biasParam;
    size_t numberOfBiases = 3;
    float biasData[] = {0.f, 1.f, -1.f};
    size_t biasDims[] = {numberOfBiases};
    size_t biasNumberOfDims = 1;
    size_t biasOrderOfDims[] = {0};
    shape_t biasShape = {
        .dimensions = biasDims,
        .numberOfDimensions = biasNumberOfDims,
        .orderOfDimensions = biasOrderOfDims
    };
    quantization_t biasQ;
    initFloat32Quantization(&biasQ);
    setTensorValues(&biasParam, biasData, &biasShape, &biasQ, NULL);

    tensor_t biasGrad;
    quantization_t biasGradQ;
    initFloat32Quantization(&biasGradQ);
    float biasGradData[] = {0.f, 0.f, 0.f};
    setTensorValues(&biasGrad, biasGradData, &biasShape, &biasQ, NULL);
    setParameterValues(&bias, &biasParam, &biasGrad);

    layer_t linear0;
    layerConfig_t linearConfig;
    linearConfig_t linCfg;
    linearConfig.linear = &linCfg;
    linearInitConfig(linearConfig.linear, &weights, &bias);
    initLayer(&linear0, LINEAR, &linearConfig, FLOAT_LAYER, NULL, NULL);

    layer_t relu0;
    initLayer(&relu0, RELU, NULL, FLOAT_LAYER, NULL, NULL);

    layer_t linear1;
    initLayer(&linear1, LINEAR, &linearConfig, FLOAT_LAYER, NULL, NULL);

    layer_t model[3] = {linear0, relu0, linear1};
    size_t sizeModel = sizeof(model) / sizeof(model[0]);
    float lr = 0.1f;
    float momentumFactor = 0.9f;
    float weightDecay = 0.5f;

    size_t sizeMomentumBuffers = calcTotalNumberOfMomentumBuffers(model, sizeModel);

    float weightMomentums0[] = {0.f, 0.f, 0.f};
    momentumBuffer_t weightMomentumBuffer0;
    initMomentumBuffer(&weightMomentumBuffer0, &weights, weightMomentums0);

    float biasMomentums0[] = {0.f, 0.f, 0.f};
    momentumBuffer_t biasMomentumBuffer0;
    initMomentumBuffer(&biasMomentumBuffer0, &bias, biasMomentums0);

    float weightMomentums1[] = {0.f, 0.f, 0.f};
    momentumBuffer_t weightMomentumBuffer1;
    initMomentumBuffer(&weightMomentumBuffer1, &weights, weightMomentums1);

    float biasMomentums1[] = {0.f, 0.f, 0.f};
    momentumBuffer_t biasMomentumBuffer1;
    initMomentumBuffer(&biasMomentumBuffer1, &bias, biasMomentums1);

    momentumBuffer_t *momentumBuffers[] = {&weightMomentumBuffer0, &biasMomentumBuffer0, &weightMomentumBuffer1, &biasMomentumBuffer1};

    SGDConfig_t sgdConfig;
    initSGDConfig(&sgdConfig, lr, momentumFactor, weightDecay, momentumBuffers, sizeMomentumBuffers);

    linearConfig_t *linear0Conf = linear0.config->linear;
    linearConfig_t *linear1Conf = linear1.config->linear;



    TEST_ASSERT_EQUAL_FLOAT(lr, sgdConfig.learningRate);
    TEST_ASSERT_EQUAL_FLOAT(momentumFactor, sgdConfig.momentumFactor);
    TEST_ASSERT_EQUAL_FLOAT(weightDecay, sgdConfig.weightDecay);
    TEST_ASSERT_EQUAL_size_t(4, sgdConfig.sizeMomentumBuffers);

    TEST_ASSERT_EQUAL_PTR(linear0Conf->weights, sgdConfig.momentumBuffers[0]->parameter);
    TEST_ASSERT_EQUAL_PTR(linear0Conf->bias, sgdConfig.momentumBuffers[1]->parameter);
    TEST_ASSERT_EQUAL_PTR(linear1Conf->weights, sgdConfig.momentumBuffers[2]->parameter);
    TEST_ASSERT_EQUAL_PTR(linear1Conf->bias, sgdConfig.momentumBuffers[3]->parameter);

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear0Conf->weights->grad->data, sgdConfig.momentumBuffers[0]->momentums,
                                  calcNumberOfElementsByParameter(linear0Conf->weights));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear0Conf->bias->grad->data, sgdConfig.momentumBuffers[1]->momentums,
                                  calcNumberOfElementsByParameter(linear0Conf->bias));

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear1Conf->weights->grad->data, sgdConfig.momentumBuffers[2]->momentums,
                              calcNumberOfElementsByParameter(linear1Conf->weights));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(linear1Conf->bias->grad->data, sgdConfig.momentumBuffers[3]->momentums,
                                  calcNumberOfElementsByParameter(linear1Conf->bias));
}


void unitTestSGDStep() {
    parameter_t weights;

    tensor_t weightParam;
    size_t numberOfWeights = 3;
    float weightData[] = {1.f, 2.f, -3.f};
    size_t weightDims[] = {numberOfWeights};
    size_t weightNumberOfDims = 1;
    size_t weightOrderOfDims[] = {0};
    shape_t weightShape = {
        .dimensions = weightDims,
        .numberOfDimensions = weightNumberOfDims,
        .orderOfDimensions = weightOrderOfDims
    };
    quantization_t weightQ;
    initFloat32Quantization(&weightQ);
    setTensorValues(&weightParam, weightData, &weightShape, &weightQ, NULL);

    tensor_t weightGrad;
    float weightGradData[] = {1.f, -1.f, 2.f};
    quantization_t weightGradQ;
    initFloat32Quantization(&weightGradQ);
    setTensorValues(&weightGrad, weightGradData, &weightShape, &weightGradQ, NULL);
    setParameterValues(&weights, &weightParam, &weightGrad);

    parameter_t bias;
    tensor_t biasParam;
    size_t numberOfBiases = 2;
    float biasData[] = {-1.f, 3.f};
    size_t biasDims[] = {numberOfBiases};
    size_t biasNumberOfDims = 1;
    size_t biasOrderOfDims[] = {0};
    shape_t biasShape = {
        .dimensions = biasDims,
        .numberOfDimensions = biasNumberOfDims,
        .orderOfDimensions = biasOrderOfDims
    };
    quantization_t biasQ;
    initFloat32Quantization(&biasQ);
    setTensorValues(&biasParam, biasData, &biasShape, &biasQ, NULL);

    tensor_t biasGrad;
    quantization_t biasGradQ;
    initFloat32Quantization(&biasGradQ);
    float biasGradData[] = {1.f, 3.f};
    setTensorValues(&biasGrad, biasGradData, &biasShape, &biasQ, NULL);
    setParameterValues(&bias, &biasParam, &biasGrad);

    layer_t linear0;
    layerConfig_t linearConfig;
    linearConfig_t linCfg;
    linearConfig.linear = &linCfg;
    linearInitConfig(linearConfig.linear, &weights, &bias);
    initLayer(&linear0, LINEAR, &linearConfig, FLOAT_LAYER, NULL, NULL);

    layer_t model[] = {linear0};
    float lr = 0.1f;
    float momentumFactor = 0.9f;
    float weightDecay = 0.01f;

    size_t sizeMomentumBuffers = calcTotalNumberOfMomentumBuffers(model, 1);

    tensor_t momentumWeights;
    float momentumWeightsData[] = {0.f, 0.f, 0.f};
    quantization_t momentumWeightsQ;
    initFloat32Quantization(&momentumWeightsQ);
    setTensorValues(&momentumWeights, momentumWeightsData, &weightShape, &momentumWeightsQ, NULL);

    tensor_t momentumBias;
    float momentumBiasData[] = {0.f, 0.f, 0.f};
    quantization_t momentumBiasQ;
    initFloat32Quantization(&momentumBiasQ);
    setTensorValues(&momentumBias, momentumBiasData, &biasShape, &momentumBiasQ, NULL);

    momentumBuffer_t weightMomentumBuffer;
    weightMomentumBuffer.parameter = &weights;
    weightMomentumBuffer.momentums = &momentumWeights;

    momentumBuffer_t biasMomentumBuffer;
    biasMomentumBuffer.parameter = &bias;
    biasMomentumBuffer.momentums = &momentumBias;

    momentumBuffer_t *momentumBuffers[] = {&weightMomentumBuffer, &biasMomentumBuffer};

    SGDConfig_t config;
    initSGDConfig(&config, lr, momentumFactor, weightDecay, momentumBuffers, sizeMomentumBuffers);

    SGDStepFloat(&config);


    float wPExpected[] = {0.899f, 2.098f, -3.197f};
    float bPExpected[] = {-1.099f, 2.697f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wPExpected, linearConfig.linear->weights->param->data,
                                  sizeof(wPExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bPExpected, linearConfig.linear->bias->param->data,
                                  sizeof(bPExpected)/sizeof(float));

    // Second Step with same grads but with momentum now
    SGDStepFloat(&config);

    float wPExpected2[] = {0.707201f, 2.284102f, -3.571103f};
    float bPExpected2[] = {-1.287001f, 2.121603f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wPExpected2, linearConfig.linear->weights->param->data,
                                  sizeof(wPExpected2)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bPExpected2, linearConfig.linear->bias->param->data,
                                  sizeof(bPExpected2)/sizeof(float));
}

void unitTestSGDZeroGrad() {
    parameter_t weights;

    tensor_t weightParam;
    size_t numberOfWeights = 3;
    float weightData[] = {1.f, 2.f, -3.f};
    size_t weightDims[] = {numberOfWeights};
    size_t weightNumberOfDims = 1;
    size_t weightOrderOfDims[] = {0};
    shape_t weightShape = {
        .dimensions = weightDims,
        .numberOfDimensions = weightNumberOfDims,
        .orderOfDimensions = weightOrderOfDims
    };
    quantization_t weightQ;
    initFloat32Quantization(&weightQ);
    setTensorValues(&weightParam, weightData, &weightShape, &weightQ, NULL);

    tensor_t weightGrad;
    float weightGradData[] = {1.f, -1.f, 2.f};
    quantization_t weightGradQ;
    initFloat32Quantization(&weightGradQ);
    setTensorValues(&weightGrad, weightGradData, &weightShape, &weightGradQ, NULL);
    setParameterValues(&weights, &weightParam, &weightGrad);

    parameter_t bias;
    tensor_t biasParam;
    size_t numberOfBiases = 2;
    float biasData[] = {-1.f, 3.f};
    size_t biasDims[] = {numberOfBiases};
    size_t biasNumberOfDims = 1;
    size_t biasOrderOfDims[] = {0};
    shape_t biasShape = {
        .dimensions = biasDims,
        .numberOfDimensions = biasNumberOfDims,
        .orderOfDimensions = biasOrderOfDims
    };
    quantization_t biasQ;
    initFloat32Quantization(&biasQ);
    setTensorValues(&biasParam, biasData, &biasShape, &biasQ, NULL);

    tensor_t biasGrad;
    quantization_t biasGradQ;
    initFloat32Quantization(&biasGradQ);
    float biasGradData[] = {1.f, 3.f};
    setTensorValues(&biasGrad, biasGradData, &biasShape, &biasQ, NULL);
    setParameterValues(&bias, &biasParam, &biasGrad);


    layer_t linear0;
    layerConfig_t linearConfig;
    linearConfig_t linCfg;
    linearConfig.linear = &linCfg;
    linearInitConfig(linearConfig.linear, &weights, &bias);
    initLayer(&linear0, LINEAR, &linearConfig, FLOAT_LAYER, NULL, NULL);

    layer_t model[] = {linear0};
    float lr = 0.1f;
    float momentumFactor = 0.9f;
    float weightDecay = 0.01f;

    size_t sizeMomentumBuffers = calcTotalNumberOfMomentumBuffers(model, 1);
    float momentumWeights[] = {0.f, 0.f, 0.f};
    float momentumBias[] = {0.f, 0.f, 0.f};


    momentumBuffer_t weightMomentumBuffer;
    weightMomentumBuffer.parameter = &weights;
    weightMomentumBuffer.momentums = momentumWeights;

    momentumBuffer_t biasMomentumBuffer;
    biasMomentumBuffer.parameter = &bias;
    biasMomentumBuffer.momentums = momentumBias;

    momentumBuffer_t *momentumBuffers[] = {&weightMomentumBuffer, &biasMomentumBuffer};

    SGDConfig_t sgdConfig;
    initSGDConfig(&sgdConfig, lr, momentumFactor, weightDecay, momentumBuffers, sizeMomentumBuffers);

    SGDZeroGrad(&sgdConfig);
    float wGradExpected[] = {0.f, 0.f, 0.f};
    float bGradExpected[] = {0.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wGradExpected, weights.grad->data, sizeof(wGradExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bGradExpected, bias.grad->data, sizeof(bGradExpected)/sizeof(float));

}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestInitMomentumBuffer);
    RUN_TEST(unitTestInitSGDConfig);
    RUN_TEST(unitTestSGDZeroGrad);
    RUN_TEST(unitTestSGDStep);
    UNITY_END();
}
