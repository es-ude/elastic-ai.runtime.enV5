#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "string.h"
#include "unity.h"

uart_inst_t *uartInstance = uart1;
int baudrate = 115200;
uint8_t dataBits = 8;
uint8_t stopBits = 1;
uint8_t txPin = 4;
uint8_t rxPin = 5;

void uartInitInternal() {
    gpio_set_function(txPin, GPIO_FUNC_UART);
    gpio_set_function(rxPin, GPIO_FUNC_UART);

    uart_init(uartInstance, baudrate);
    uart_set_hw_flow(uartInstance, false, false);

    uart_set_format(uartInstance, dataBits, stopBits, UART_PARITY_NONE);
    uart_set_fifo_enabled(uartInstance, true);
}

void receiveResponse(char *response, uint bufferSize) {
    int receivedChars = 0;
    char *buffer = response;
    while (uart_is_readable(uartInstance) && receivedChars < bufferSize) {
        char receivedCharacter = uart_getc(uartInstance);
        buffer[receivedChars++] = receivedCharacter;
    }
}

void writeToEsp(const char *command) {
    uart_puts(uartInstance, command);
}

#define paramTest(fn, param)                                                                       \
    void fn##param(void) {                                                                         \
        fn(param);                                                                                 \
    }

static void checkATStartAndSleepBeforeReceiveForMs(int sleepMs) {
    uartInitInternal();
    char response[16] = {0};

    char *test_at_startup_command = "AT\r\n";
    writeToEsp(test_at_startup_command);
    sleep_ms(sleepMs);
    receiveResponse(response, sizeof(response));
    TEST_ASSERT_EQUAL_STRING("AT\r\n\r\nOK\r\n", response);
}

void cannotTalkToEspIfOnlySleepingForMsBeforeReceive(int sleepMs) {
    uartInitInternal();
    char response[16] = {0};

    char *test_at_startup_command = "AT\r\n";
    writeToEsp(test_at_startup_command);
    sleep_ms(sleepMs);
    receiveResponse(response, sizeof(response));
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, strcmp("AT\r\n\r\nOK\r\n", response),
                                  "expected response to be incomplete");
}

paramTest(cannotTalkToEspIfOnlySleepingForMsBeforeReceive, 1)
    paramTest(cannotTalkToEspIfOnlySleepingForMsBeforeReceive, 2)
        paramTest(checkATStartAndSleepBeforeReceiveForMs, 3)
            paramTest(checkATStartAndSleepBeforeReceiveForMs, 4)
                paramTest(checkATStartAndSleepBeforeReceiveForMs, 5)
                    paramTest(checkATStartAndSleepBeforeReceiveForMs, 6)
                        paramTest(checkATStartAndSleepBeforeReceiveForMs, 7)
                            paramTest(checkATStartAndSleepBeforeReceiveForMs, 8)
                                paramTest(checkATStartAndSleepBeforeReceiveForMs, 9)
                                    paramTest(checkATStartAndSleepBeforeReceiveForMs, 10)
                                        paramTest(checkATStartAndSleepBeforeReceiveForMs, 11)

                                            void init(void) {
    stdio_init_all();
    while (!stdio_usb_connected()) {}
}

void setUp(void) {}
void tearDown(void) {}

void deInit(void) {
    rom_reset_usb_boot(0, 0);
}

int main() {
    init();
    UNITY_BEGIN();
    RUN_TEST(cannotTalkToEspIfOnlySleepingForMsBeforeReceive1);
    RUN_TEST(cannotTalkToEspIfOnlySleepingForMsBeforeReceive2);
    RUN_TEST(checkATStartAndSleepBeforeReceiveForMs3);
    RUN_TEST(checkATStartAndSleepBeforeReceiveForMs4);
    RUN_TEST(checkATStartAndSleepBeforeReceiveForMs5);
    RUN_TEST(checkATStartAndSleepBeforeReceiveForMs6);
    RUN_TEST(checkATStartAndSleepBeforeReceiveForMs7);
    RUN_TEST(checkATStartAndSleepBeforeReceiveForMs8);
    RUN_TEST(checkATStartAndSleepBeforeReceiveForMs9);
    RUN_TEST(checkATStartAndSleepBeforeReceiveForMs10);
    RUN_TEST(checkATStartAndSleepBeforeReceiveForMs11);
    UNITY_END();
    deInit();
}
