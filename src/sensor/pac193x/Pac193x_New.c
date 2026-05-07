#include "eai/sensor/Pac193x_New.h"
#include <stdio.h>


#define PAC193X_REG_REFRESH     0x00        // 0 bytes
#define PAC193X_REG_CONTROL     0x01        // 1 byte
#define PAC193X_REG_ACC_CNT     0x02        // 3 bytes
#define PAC193X_REG_VPOWER_ACC  0x03        // 6 bytes
#define PAC193X_REG_VBUS        0x07        // 2 bytes
#define PAC193X_REG_VSENSE      0x0B        // 2 bytes
#define PAC193X_REG_VBUS_AVG    0x0F        // 2 bytes
#define PAC193X_REG_VSENSE_AVG  0x13        // 2 bytes
#define PAC193X_REG_VPOWER      0x17        // 4 bytes
#define PAC193X_REG_ENABLE      0x1C        // 1 byte
#define PAC193X_REG_POLARITY    0x1D        // 1 byte
#define PAC193X_REG_PID         0xFD        // 1 bytes
#define PAC193X_REG_MID         0xFE        // 1 bytes
#define PAC193X_REG_RID         0xFF        // 1 bytes


// ======================= INTERNAL FUNCS =======================
bool pac193x_i2c_write(pac193x_t *config, uint8_t *data, size_t len){
    return construct_i2c_write_data(config->i2c, config->adr, data, len);
}


bool pac193x_i2c_read(pac193x_t *config, uint8_t *data_tx, size_t len_tx, uint8_t *data_rx, size_t len_rx){
    return construct_i2c_read_data(config->i2c, config->adr, data_tx, len_tx, data_rx, len_rx);
}


bool pac193x_send_refresh(pac193x_t *config){
    uint8_t cmd[1] = {0x00};
    cmd[0] = PAC193X_REG_REFRESH;

    return pac193x_i2c_write(config, cmd, sizeof(cmd));
}


// ======================= CALLABLE FUNCS =======================
uint8_t pac193x_get_i2c_address(uint32_t resistor_value){
    switch(resistor_value){
        case 0:         return 0x10;    //GND
        case 499:       return 0x11;
        case 806:       return 0x12;
        case 1270:      return 0x13;
        case 2050:      return 0x14;
        case 3240:      return 0x15;
        case 5230:      return 0x16;
        case 8450:      return 0x17;
        case 13300:     return 0x18;
        case 21500:     return 0x19;
        case 34000:     return 0x1A;
        case 54900:     return 0x1B;
        case 88700:     return 0x1C;
        case 140000:    return 0x1D;
        case 226000:    return 0x1E;
        default:        return 0x1F;    //VDD
    }
}


bool pac193x_check_product_id(pac193x_t *config){
    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_PID;

    uint8_t data_rx[1] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        if(config->num_channels == 1)       return data_rx[0] == 0x58;
        else if(config->num_channels == 2)  return data_rx[0] == 0x59;
        else if(config->num_channels == 3)  return data_rx[0] == 0x5A;
        else if(config->num_channels == 4)  return data_rx[0] == 0x5B;
        else return false;
    }
    return false;
}


bool pac193x_check_manufacturer_id(pac193x_t *config){
    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_MID;

    uint8_t data_rx[1] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        return data_rx[0] == 0x5D;
    }
    return false;
}


bool pac193x_check_revision_id(pac193x_t *config){
    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_RID;

    uint8_t data_rx[1] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        return data_rx[0] == 0x03;
    }
    return false;
}


uint8_t pac193x_get_number_of_channels(pac193x_t *config){
    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_PID;

    uint8_t data_rx[1] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        if(data_rx[0] == 0x58)       return 1;
        else if(data_rx[0] == 0x59)  return 2;
        else if(data_rx[0] == 0x5A)  return 3;
        else if(data_rx[0] == 0x5B)  return 4;
        else return 0;
    }
    return 0;
}


bool pac193x_set_single_shot_mode(pac193x_t *config, bool enable_single_shot_mode){
    config->enable_single_shot_mode = enable_single_shot_mode;
    return pac193x_set_sampling_rate(config, config->sample_rate);
}


bool pac193x_set_sleep_mode(pac193x_t *config, bool enable_sleep_mode){
    config->enable_sleep_mode = enable_sleep_mode;
    return pac193x_set_sampling_rate(config, config->sample_rate);
}


bool pac193x_set_sampling_rate(pac193x_t *config, uint8_t sample_rate){
    if(sample_rate > 3) {
        return false;
    }

    config->sample_rate = sample_rate;
    uint8_t data[2] = {0x00};
    data[0] = PAC193X_REG_CONTROL;
    data[1] = ((config->sample_rate & 0x03) << 6) |
        ((config->enable_sleep_mode) ? 0x20 : 0x00) |
        ((config->enable_single_shot_mode) ? 0x10 : 0x00) |
        0x0A; // Enalbing Alert Pin as OVF Alert

    pac193x_i2c_write(config, data, sizeof(data));
    return pac193x_send_refresh(config);
}


bool pac193x_enable_all_channels(pac193x_t *config, bool enable_channels){
    config->enable_channels = enable_channels;
    uint8_t data[2] = {0x00};
    data[0] = PAC193X_REG_ENABLE;
    data[1] = (config->enable_channels) ? 0x00 : 0x0F;

    pac193x_i2c_write(config, data, sizeof(data));
    return pac193x_send_refresh(config);
}


bool pac193x_polarity_voltage(pac193x_t *config, bool use_bipolar){
    config->enable_bipolar_voltage = use_bipolar;

    uint8_t data[2] = {0x00};
    data[0] = PAC193X_REG_POLARITY;
    data[1] = ((config->enable_bipolar_voltage) ? 0x0F : 0x00) |
        ((config->enable_bipolar_current) ? 0xF0 : 0x00);

    pac193x_i2c_write(config, data, sizeof(data));
    return pac193x_send_refresh(config);
}


bool pac193x_polarity_current(pac193x_t *config, bool use_bipolar){
    config->enable_bipolar_current = use_bipolar;

    uint8_t data[2] = {0x00};
    data[0] = PAC193X_REG_POLARITY;
    data[1] = ((config->enable_bipolar_voltage) ? 0x0F : 0x00) |
        ((config->enable_bipolar_current) ? 0xF0 : 0x00);

    pac193x_i2c_write(config, data, sizeof(data));
    return pac193x_send_refresh(config);
}


bool pac193x_init(pac193x_t *config){
    if(config->gpio_pwrdwn != 255){
        gpio_init(config->gpio_pwrdwn);
        gpio_set_dir(config->gpio_pwrdwn, GPIO_OUT);
        gpio_pull_up(config->gpio_pwrdwn);
        gpio_put(config->gpio_pwrdwn, true);
    }
    if(config->gpio_alert != 255){
        gpio_init(config->gpio_alert);
        gpio_set_dir(config->gpio_alert, GPIO_OUT);
        gpio_pull_up(config->gpio_alert);
        gpio_put(config->gpio_alert, false);
    }

    if(!config->i2c->init_done){
        init_i2c_module(config->i2c);
    }

    if(config->adr < PAC193X_I2C_ADDR_START || config->adr > PAC193X_I2C_ADDR_END){
        config->adr = pac193x_get_i2c_address(0);
    }
    if(!check_i2c_bus_for_device_specific(config->i2c, config->adr)){
        config->init_done = false;
        return false;
    }
    if(config->num_channels == 0){
        config->num_channels = pac193x_get_number_of_channels(config);
    }
    config->init_done = pac193x_check_product_id(config) && pac193x_check_manufacturer_id(config);
    if(!config->init_done) {
        return false;
    }

    // Send to control register (all params will be set)
    pac193x_polarity_current(config, config->enable_bipolar_current);
    pac193x_polarity_current(config, config->enable_bipolar_voltage);
    pac193x_set_sampling_rate(config, config->sample_rate);
    pac193x_enable_all_channels(config, config->enable_channels);
    return config->init_done;
}


bool pac193x_update_data_register(pac193x_t *config){
    pac193x_send_refresh(config);
    return true;
}


uint16_t pac193x_read_voltage(pac193x_t *config, uint8_t channel){
    if(config->num_channels == 0)
        config->num_channels = pac193x_get_number_of_channels(config);
    if(channel > config->num_channels - 1)
        return 0;

    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_VBUS + channel;

    uint8_t data_rx[2] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        return (data_rx[0] << 8) | (data_rx[1] << 0);
    }
    return 0;
}


uint16_t pac193x_read_voltage_rolling(pac193x_t *config, uint8_t channel){
    if(config->num_channels == 0)
        config->num_channels = pac193x_get_number_of_channels(config);
    if(channel > config->num_channels - 1)
        return 0;

    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_VBUS_AVG + channel;

    uint8_t data_rx[2] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        return (data_rx[0] << 8) | (data_rx[1] << 0);
    }
    return 0;
}


uint16_t pac193x_read_current(pac193x_t *config, uint8_t channel){
    if(config->num_channels == 0)
        config->num_channels = pac193x_get_number_of_channels(config);
    if(channel > config->num_channels - 1)
        return 0;

    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_VSENSE + channel;

    uint8_t data_rx[2] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        return (data_rx[0] << 8) | (data_rx[1] << 0);
    }
    return 0;
}


uint16_t pac193x_read_current_rolling(pac193x_t *config, uint8_t channel){
    if(config->num_channels == 0)
        config->num_channels = pac193x_get_number_of_channels(config);
    if(channel > config->num_channels - 1)
        return 0;

    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_VSENSE_AVG + channel;

    uint8_t data_rx[2] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        return (data_rx[0] << 8) | (data_rx[1] << 0);
    }
    return 0;
}


uint32_t pac193x_read_power(pac193x_t *config, uint8_t channel){
    if(config->num_channels == 0)
        config->num_channels = pac193x_get_number_of_channels(config);
    if(channel > config->num_channels - 1)
        return 0;

    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_VPOWER + channel;

    uint8_t data_rx[4] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        return (data_rx[0] << 20) | (data_rx[1] << 12) | (data_rx[2] << 4) | (data_rx[3] >> 4);
    }
    return 0;
}


uint64_t pac193x_read_power_accumulated(pac193x_t *config, uint8_t channel){
    if(config->num_channels == 0)
        config->num_channels = pac193x_get_number_of_channels(config);
    if(channel > config->num_channels - 1)
        return 0;

    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_VPOWER_ACC + channel;

    uint8_t data_rx[6] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        return ((uint64_t)data_rx[0] << 40) | ((uint64_t)data_rx[1] << 32) | ((uint64_t)data_rx[2] << 24) | ((uint64_t)data_rx[3] << 16) | ((uint64_t)data_rx[4] << 8) | (data_rx[5] << 0);
    }
    return 0;
}


uint32_t pac193x_read_accumulation_number(pac193x_t *config){
    uint8_t data_tx[1] = {0x00};
    data_tx[0] = PAC193X_REG_ACC_CNT;

    uint8_t data_rx[3] = {0x00};
    if(pac193x_i2c_read(config, data_tx, sizeof(data_tx), data_rx, sizeof(data_rx))){
        return (data_rx[0] << 16) | (data_rx[1] << 8) | (data_rx[2] << 0);
    }
    return 0;
}


bool pac193x_read_all_voltages(pac193x_t *config, uint16_t *out, size_t len) {
    if (len < config->num_channels) return false;

    uint8_t data_tx[1] = {PAC193X_REG_VBUS};
    uint8_t data_rx[2 * 4] = {0};  // 2 bytes * 4 Kanäle

    if (pac193x_i2c_read(config, data_tx, 1, data_rx, 2 * config->num_channels)) {
        for (uint8_t i = 0; i < config->num_channels; i++) {
            out[i] = (data_rx[2*i] << 8) | data_rx[2*i + 1];
        }
        return true;
    }
    return false;
}

bool pac193x_read_all_currents(pac193x_t *config, int16_t *out, size_t len) {
    if (len < config->num_channels) return false;

    uint8_t data_tx[1] = {PAC193X_REG_VSENSE};
    uint8_t data_rx[2 * 4] = {0};

    if (pac193x_i2c_read(config, data_tx, 1, data_rx, 2 * config->num_channels)) {
        for (uint8_t i = 0; i < config->num_channels; i++) {
            out[i] = (data_rx[2*i] << 8) | data_rx[2*i + 1];
        }
        return true;
    }
    return false;
}

bool pac193x_read_all_power(pac193x_t *config, uint32_t *out, size_t len) {
    if (len < config->num_channels) return false;

    uint8_t data_tx[1] = {PAC193X_REG_VPOWER};
    uint8_t data_rx[4 * 4] = {0};  // 4 bytes * 4 Kanäle

    if (pac193x_i2c_read(config, data_tx, 1, data_rx, 4 * config->num_channels)) {
        for (uint8_t i = 0; i < config->num_channels; i++) {
            out[i] = ((uint32_t)data_rx[4*i]     << 20) |
                     ((uint32_t)data_rx[4*i + 1] << 12) |
                     ((uint32_t)data_rx[4*i + 2] <<  4) |
                     (          data_rx[4*i + 3] >>  4);
        }
        return true;
    }
    return false;
}

bool pac193x_read_all_power_accumulated(pac193x_t *config, uint64_t *out, size_t len) {
    if (len < config->num_channels) return false;

    uint8_t data_tx[1] = {PAC193X_REG_VPOWER_ACC};
    uint8_t data_rx[6 * 4] = {0};  // 6 bytes * 4 Kanäle

    if (pac193x_i2c_read(config, data_tx, 1, data_rx, 6 * config->num_channels)) {
        for (uint8_t i = 0; i < config->num_channels; i++) {
            out[i] = ((uint64_t)data_rx[6*i]     << 40) |
                     ((uint64_t)data_rx[6*i + 1] << 32) |
                     ((uint64_t)data_rx[6*i + 2] << 24) |
                     ((uint64_t)data_rx[6*i + 3] << 16) |
                     ((uint64_t)data_rx[6*i + 4] <<  8) |
                     (          data_rx[6*i + 5] <<  0);
        }
        return true;
    }
    return false;
}
