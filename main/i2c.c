//
// Created by yekai on 2021/8/6.
//

#include "i2c.h"
#include "driver/i2c.h"

void MY_I2C_Init() {
    i2c_config_t conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = FM_SDA_Pin,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_io_num = FM_SCL_Pin,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = 100 * 1000,
    };
    ESP_ERROR_CHECK(i2c_param_config(FM_I2C_HOST, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(FM_I2C_HOST, conf.mode, 0, 0, 0));
}