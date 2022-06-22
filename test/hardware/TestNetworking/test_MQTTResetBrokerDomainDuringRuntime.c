#define SOURCE_FILE "RESET-BROKER-TEST"

#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"

#include "hardwareTestHelper.h"
#include "MQTTBroker.h"
#include "QueueWrapper.h"
#include "TaskWrapper.h"
#include "protocol.h"
#include "esp.h"
#include "common.h"

void enterBootModeTask(void);

void init(void);

void _Noreturn mainTask(void) {
    PRINT("=== STARTING TEST ===")

    connectToNetwork();

    while (true) {
        connectToMQTT();
        publishData("testENv5Pub", "data");
        TaskSleep(5000);
        MQTT_Broker_setBrokerDomain("second_device_with_longer_name");
    }
}

int main() {
    init();

    RegisterTask(enterBootModeTask, "enterBootModeTask");
    RegisterTask(mainTask, "mainTask");
    StartScheduler();
}

void init(void) {
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    if (watchdog_enable_caused_reboot()) {
        reset_usb_boot(0, 0);
    }
    ESP_Init();
    CreateQueue();
    watchdog_enable(2000, 1);
}

void _Noreturn enterBootModeTask(void) {
    while (true) {
        if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
            MQTT_Broker_Disconnect(true);//
            reset_usb_boot(0, 0);
        }
        watchdog_update();
        TaskSleep(1000);
    }
}
