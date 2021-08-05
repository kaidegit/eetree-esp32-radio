//
// Created by yekai on 2021/8/5.
//

#include "gpio.h"
#include "driver/gpio.h"
#include "main.h"

void MY_GPIO_Init() {
    gpio_set_direction(OLED_DC_Pin,GPIO_MODE_OUTPUT);
    gpio_set_direction(OLED_RST_Pin,GPIO_MODE_OUTPUT);
}

