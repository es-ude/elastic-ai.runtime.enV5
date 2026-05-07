#include <stdio.h>
#include "eai/sensor/Pac193x_New.h"
#include "eai/hal/I2cPico.h"
#include "eai/hal/EnV5HwController.h"

static uint8_t toggleLeds(uint8_t toggle) {
    if(toggle) {
        env5HwControllerLedsAllOn();
        return 0;
    }
    else {
        env5HwControllerLedsAllOff();
        return 1;
    }
}

int main(){
    stdio_init_all();
    sleep_ms(1000);

    uint8_t toggle = 1;

    env5HwControllerInit();
    env5HwControllerLedsInit();

    // --- Init of the I2C---
    static i2c_rp2_t i2c0_inst = {
        .i2c_mod = i2c1,
        .pin_sda = 6,
        .pin_scl = 7,
        .fi2c_khz = 100,
        .avai_devices = 0,
        .init_done = false,
    };
    init_i2c_module(&i2c0_inst);
    scan_i2c_bus_for_device(&i2c0_inst);

    // --- Init of the PAC193x ---
    static pac193x_t pac193x_config = {
        .i2c = &i2c0_inst,
        .gpio_pwrdwn = 255,
        .gpio_alert = 255,
        .adr = 0,
        .num_channels = 0,
        .sample_rate = 3,
        .enable_channels = true,
        .enable_sleep_mode = false,
        .enable_single_shot_mode = false,
        .init_done = false
    };
    pac193x_config.adr = pac193x_get_i2c_address(499);

    if(pac193x_init(&pac193x_config)){
        printf("Init PAC193x sensor done\n");
    }

    uint16_t voltage[4]    = {0};
    int16_t current[4]    = {0};
    uint32_t power[4]      = {0};
    uint64_t power_acc[4]  = {0};

    while (true) {
        sleep_ms(2000);
        pac193x_update_data_register(&pac193x_config);

        uint32_t accumulation_number = pac193x_read_accumulation_number(&pac193x_config);
        pac193x_read_all_voltages(&pac193x_config, voltage, 4);
        pac193x_read_all_currents(&pac193x_config, current, 4);
        pac193x_read_all_power(&pac193x_config, power, 4);
        pac193x_read_all_power_accumulated(&pac193x_config, power_acc, 4);

        printf("===========Data=======\n");
        printf("Ite #: %lu\n", accumulation_number);
        for (uint8_t i = 0; i < pac193x_config.num_channels; i++) {
            printf("--- CH%d ---\n", i);
            printf("V:       %u\n",   voltage[i]);
            printf("I:       %d\n",   current[i]);
            printf("P:       %lu\n",  power[i]);
            printf("P-ACC:   %llu\n", power_acc[i]);
        }
        toggle = toggleLeds(toggle);
    }
}

