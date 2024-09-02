#define SOURCE_FILE "STUB"

#include "stub.h"
#include "Sleep.h"
#include "middleware.h"
#include "stub_defs.h"

#ifndef ADDR_COMPUTATION_ENABLE
#error ADDR_COMPUTATION_ENABLE not defined!
#endif
#ifndef ADDR_SKELETON_INPUTS
#error ADDR_SKELETON_INPUTS not defined!
#endif
#ifndef ADDR_MODEL_ID
#error ADDR_MODEL_ID not defined!
#endif
#ifndef BYTES_MODEL_ID
#error BYTES_MODEL_ID not defined!
#endif

static void startCompute(void);
static void stopCompute(void);

bool modelDeploy(uint32_t acceleratorAddress, const uint8_t *acceleratorId) {
    middlewareInit();
    middlewareConfigureFpga(acceleratorAddress);
    middlewareDeinit();
    sleep_for_ms(200);
    uint8_t id[BYTES_MODEL_ID];
    modelGetId(id);

    for (size_t index = 0; index < BYTES_MODEL_ID; index++) {
        if (id[index] != acceleratorId[index]) {
            return false;
        }
    }
    return true;
}

void modelPredict(int8_t *inputs, size_t num_inputs, int8_t *result, size_t num_results) {
    middlewareInit();
    middlewareUserlogicEnable();

    // send input
    middlewareWriteBlocking(ADDR_SKELETON_INPUTS + 0, (uint8_t *)(inputs), num_inputs);

    // run computation
    startCompute();
    while (middlewareUserlogicGetBusyStatus()) {}
    stopCompute();

    // read output
    for (int i = 0; i < num_results; i++) {
        middlewareReadBlocking(ADDR_SKELETON_INPUTS + 0 + i, (uint8_t *)(&result) + i, 1);
        middlewareReadBlocking(ADDR_SKELETON_INPUTS + 0 + i, (uint8_t *)(&result) + i, 1);
    }

    middlewareUserlogicDisable();
    middlewareDeinit();
}

void modelGetId(uint8_t *id) {
    middlewareInit();
    middlewareUserlogicEnable();
    middlewareReadBlocking(ADDR_MODEL_ID, id, BYTES_MODEL_ID);
    middlewareUserlogicDisable();
    middlewareDeinit();
}

static void startCompute(void) {
    uint8_t cmd[1] = {0x01};
    middlewareWriteBlocking(ADDR_COMPUTATION_ENABLE, cmd, 1);
}
static void stopCompute(void) {
    uint8_t cmd[1] = {0x00};
    middlewareWriteBlocking(ADDR_COMPUTATION_ENABLE, cmd, 1);
}
