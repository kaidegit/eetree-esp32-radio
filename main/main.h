//
// Created by yekai on 2021/8/5.
//

#ifndef SMART_CONFIG_MAIN_H
#define SMART_CONFIG_MAIN_H

#include <hal/spi_types.h>
#include "esp_system.h"

// OLED pins config
#define OLED_CLK_Pin    36
#define OLED_MOSI_Pin   35
#define OLED_RST_Pin    34
#define OLED_DC_Pin     33
#define OLED_SPI_HOST   FSPI_HOST

// FM Module pins config
#define FM_SCL_Pin      4
#define FM_SDA_Pin      5

// MUX pin config
#define MUX_Pin         42

#endif //SMART_CONFIG_MAIN_H
