//
// Created by yekai on 2021/8/5.
//

#ifndef SMART_CONFIG_MAIN_H
#define SMART_CONFIG_MAIN_H

#include <hal/spi_types.h>
#include "esp_system.h"
#include "stdbool.h"

//Key pins config
#define Key1_Pin        1
#define Key2_Pin        2
#define Key3_Pin        3
#define Key4_Pin        6

// OLED pins config
#define OLED_CLK_Pin    36
#define OLED_MOSI_Pin   35
#define OLED_RST_Pin    34
#define OLED_DC_Pin     33
#define OLED_SPI_HOST   FSPI_HOST

// FM Module pins config
#define FM_SCL_Pin      4
#define FM_SDA_Pin      5
#define FM_I2C_HOST     I2C_NUM_0

// MUX pin config
#define MUX_Pin         42

// AUDIO Pin config
#define AUDIO_Pin       17
#define SPEAKER_EN_Pin  41



#endif //SMART_CONFIG_MAIN_H
