#define SOURCE_FILE "SGD-UTEST"

#include "AiHelpers.h"
#include "SGD.h"
#include "unity.h"
#include <stdlib.h>

void setUp() {}
void tearDown() {}

void unitTestSGDStep() {
    float wP[] = {1.f, 2.f, -3.f};
    float wGrad[] = {1.f, -1.f, 2.f};
    float wMomentum[] = {0.f, 0.f, 0.f}; //zeros size of P

    float bP[] = {-1.f, 3.f};
    float bGrad[] = {1.f, 3.f};
    float bMomentum[] = {0.f, 0.f}; //zeros size of P

    parameter_t weight;
    weight.p = wP;
    weight.grad = wGrad;
    weight.size = sizeof(wP)/sizeof(float);

    parameter_t bias;
    bias.p = bP;
    bias.grad = bGrad;
    bias.size = sizeof(bP)/sizeof(float);

    momentumBuffer_t wMomentumBuffer;
    wMomentumBuffer.parameter = &weight;
    wMomentumBuffer.momentums = wMomentum;

    momentumBuffer_t bMomentumBuffer;
    bMomentumBuffer.parameter = &bias;
    bMomentumBuffer.momentums = bMomentum;

    momentumBuffer_t momentumBuffer[] = {wMomentumBuffer, bMomentumBuffer};

    SGDConfig_t config;
    config.lr = 0.1f;
    config.momentum = 0.9f;
    config.weightDecay = 0.01f;
    config.momentum_buffer = momentumBuffer;
    config.sizeMomentumBuffers = sizeof(momentumBuffer)/sizeof(momentumBuffer_t);

    SGDStep(&config);

    float wPExpected[] = {0.899f, 2.098f, -3.197f};
    float bPExpected[] = {-1.099f, 2.697f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wP, wPExpected, sizeof(wPExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bP, bPExpected, sizeof(bPExpected)/sizeof(float));

    //Second Step with same grads but with momentum now
    SGDStep(&config);

    float wPExpected2[] = {0.707201f, 2.286802f, -3.576503f};
    float bPExpected2[] = {-1.288801f, 2.127003f};

    for (size_t i = 0; i < sizeof(wP)/sizeof(float); i++) {
        int expected = (int)(wPExpected2[i]*100);
        int actual   = (int)(wP[i]*100);
        TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, "Die ersten zwei Nachkommastellen stimmen nicht überein");
    }

    for (size_t i = 0; i < sizeof(bP)/sizeof(float); i++) {
        int expected = (int)(bPExpected2[i]*100);
        int actual   = (int)(bP[i]*100);
        TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, "Die ersten zwei Nachkommastellen stimmen nicht überein");
    }


}

void unitTestSGDZeroGrad() {
    float wP[] = {1.f, 2.f, -3.f};
    float wGrad[] = {1.f, -1.f, 2.f};
    float wMomentum[] = {0.f, 0.f, 0.f}; //zeros size of P

    float bP[] = {-1.f, 3.f};
    float bGrad[] = {1.f, 3.f};
    float bMomentum[] = {0.f, 0.f}; //zeros size of P

    parameter_t weight;
    weight.p = wP;
    weight.grad = wGrad;
    weight.size = sizeof(wP)/sizeof(float);

    parameter_t bias;
    bias.p = bP;
    bias.grad = bGrad;
    bias.size = sizeof(bP)/sizeof(float);

    momentumBuffer_t wMomentumBuffer;
    wMomentumBuffer.parameter = &weight;
    wMomentumBuffer.momentums = wMomentum;

    momentumBuffer_t bMomentumBuffer;
    bMomentumBuffer.parameter = &bias;
    bMomentumBuffer.momentums = bMomentum;

    momentumBuffer_t momentumBuffer[] = {wMomentumBuffer, bMomentumBuffer};

    SGDConfig_t config;
    config.lr = 0.1f;
    config.momentum = 0.9f;
    config.weightDecay = 0.01f;
    config.momentum_buffer = momentumBuffer;
    config.sizeMomentumBuffers = sizeof(momentumBuffer) / sizeof(momentumBuffer_t);

    SGDZeroGrad(&config);
    float wGradExpected[] = {0.f, 0.f, 0.f};
    float bGradExpected[] = {0.f, 0.f};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(wGrad, wGradExpected, sizeof(wGradExpected)/sizeof(float));
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(bGrad, bGradExpected, sizeof(bGradExpected)/sizeof(float));

}

int main() {
    UNITY_BEGIN();
    RUN_TEST(unitTestSGDStep);
    RUN_TEST(unitTestSGDZeroGrad);
    UNITY_END();
}