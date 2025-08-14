#define SOURCE_FILE "FREERTOS-HWTEST"

#include <stdlib.h>
#include <string.h>

#include "pico/bootrom.h"
#include "pico/stdio_usb.h"

#include "unity.h"

#include "eai/Common.h"
#include "eai/hal/EnV5HwController.h"
#include "eai/rtos/FreeRtosQueueWrapper.h"
#include "eai/rtos/FreeRtosTaskWrapper.h"

#define QUEUE_SIZE 5

typedef struct message {
    uint16_t messageId;
    char content[15];
} message_t;

queue_t messageQueue;
char staticMessage[] = "STATIC MSG";
char dynamicMessage[] = "DYNAMIC MSG";

void init(void) {
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    env5HwControllerInit();
}

void testCreateQueueSuccessful(void) {
    TEST_ASSERT_NOT_NULL(messageQueue);
}

void testQueueInitiallyEmpty(void) {
    message_t tempMessage;
    TEST_ASSERT_FALSE(freeRtosQueueWrapperPop(messageQueue, &tempMessage));
}

void testPushAndPopSingleMessage() {
    message_t sentMessage = {.messageId = 1};
    strcpy(sentMessage.content, staticMessage);

    TEST_ASSERT_TRUE(freeRtosQueueWrapperPush(messageQueue, &sentMessage));

    message_t receivedMessage;
    TEST_ASSERT_TRUE(freeRtosQueueWrapperPop(messageQueue, &receivedMessage));
    TEST_ASSERT_EQUAL_UINT16(1, receivedMessage.messageId);
    TEST_ASSERT_EQUAL_STRING(staticMessage, receivedMessage.content);
}

void testModifyMessageBeforePush() {
    message_t sentMessage = {.messageId = 3};
    strcpy(sentMessage.content, staticMessage);
    if (sentMessage.messageId % 2) {
        strcpy(sentMessage.content, dynamicMessage);
    }

    TEST_ASSERT_TRUE(freeRtosQueueWrapperPush(messageQueue, &sentMessage));

    message_t receivedMessage;
    TEST_ASSERT_TRUE(freeRtosQueueWrapperPop(messageQueue, &receivedMessage));
    TEST_ASSERT_EQUAL_UINT16(3, receivedMessage.messageId);
    TEST_ASSERT_EQUAL_STRING(dynamicMessage, receivedMessage.content);
}

void testQueueHandlesMaxCapacity() {
    message_t message;
    for (int i = 0; i < QUEUE_SIZE; i++) {
        message.messageId = i;
        strcpy(message.content, staticMessage);
        TEST_ASSERT_TRUE(freeRtosQueueWrapperPush(messageQueue, &message));
    }

    message.messageId = QUEUE_SIZE + 1;
    strcpy(message.content, staticMessage);
    TEST_ASSERT_FALSE(freeRtosQueueWrapperPush(messageQueue, &message));
}

void setUp() {}
void tearDown() {}
void deInit() {
    rom_reset_usb_boot(0, 0);
}

void testRunnerTask() {
    UNITY_BEGIN();
    RUN_TEST(testCreateQueueSuccessful);
    RUN_TEST(testQueueInitiallyEmpty);
    RUN_TEST(testPushAndPopSingleMessage);
    RUN_TEST(testModifyMessageBeforePush);
    RUN_TEST(testQueueHandlesMaxCapacity);
    UNITY_END();
    deInit();
}

int main(void) {
    init();
    messageQueue = freeRtosQueueWrapperCreate(QUEUE_SIZE, sizeof(message_t));
    freeRtosTaskWrapperRegisterTask(testRunnerTask, "test_runner", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperStartScheduler();
}
